#include "pxpch.h"

#include "Platform/DirectX/DirectXDevice.h"
#include "CommandQueue.h"

namespace Pixel {

	//------Command Alloactor Pool------
	CommandAllocatorPool::CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type)
		:m_CommandListType(Type)
	{

	}

	CommandAllocatorPool::~CommandAllocatorPool()
	{
		ShutDown();
	}

	void CommandAllocatorPool::ShutDown()
	{
		m_AlloactorPool.clear();
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocatorPool::RequestAlloactor(uint64_t CompletedFenceValue)
	{
		std::lock_guard<std::mutex> LockGuard(m_AlloactorMutex);

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator;

		if (!m_ReadyAlloactors.empty())
		{
			std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>& AllocatorPair = m_ReadyAlloactors.front();

			if (AllocatorPair.first <= CompletedFenceValue)
			{
				pAllocator = AllocatorPair.second;
				PX_CORE_ASSERT(pAllocator->Reset() >= 0, "Command Allocator's Reset Error!")
				m_ReadyAlloactors.pop();
			}
		}

		//if allocator were not ready to be reused, create a new one
		if (pAllocator == nullptr)
		{
			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(pAllocator.ReleaseAndGetAddressOf())) >= 0,
				"Create Command Alloactor Error!");

			//------debug name------
			wchar_t AlloactorName[32];
			swprintf(AlloactorName, 32, L"CommandAlloactor %zu", m_AlloactorPool.size());
			pAllocator->SetName(AlloactorName);
			//------debug name------
			m_AlloactorPool.push_back(pAllocator);
		}

		return pAllocator;
	}

	void CommandAllocatorPool::DiscardAlloactor(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Alloactor)
	{
		std::lock_guard<std::mutex> LockGuard(m_AlloactorMutex);

		m_ReadyAlloactors.push(std::make_pair(FenceValue, Alloactor));
	}
	//------Command Alloactor Pool------

	//------Command Queue------
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type)
		:m_Type(Type),
		m_NextFenceValue((uint64_t)Type << 56 | 1),
		m_LastCompletedFenceValue((uint64_t)Type << 56),
		m_pAlloactorPool(Type)
	{

	}

	CommandQueue::~CommandQueue()
	{
	}

	void CommandQueue::Create()
	{

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Type = m_Type;
		QueueDesc.NodeMask = 1;
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(m_pCommandQueue.ReleaseAndGetAddressOf())) >= 0,
			"Create Command Queue Error!");
		m_pCommandQueue->SetName(L"CommandListManager::m_CommandQueue");

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())) >= 0,
		"Create Fence Error!");

		m_pFence->SetName(L"CommandListManager::m_Fence");
		//the highest 8 bit is type
		m_pFence->Signal((uint64_t)m_Type << 56);

		m_FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);

		PX_CORE_ASSERT(m_FenceEventHandle != NULL, "Create Fence Error!");
	}

	void CommandQueue::ShutDown()
	{
		CloseHandle(m_FenceEventHandle);
	}

	void CommandQueue::StallForFence(uint64_t FenceValue)
	{
		CommandQueue& Producer = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->GetQueue((D3D12_COMMAND_LIST_TYPE)(FenceValue >> 56));

		m_pCommandQueue->Wait(Producer.m_pFence.Get(), FenceValue);
	}

	void CommandQueue::StallForProducer(CommandQueue& Producer)
	{
		m_pCommandQueue->Wait(Producer.m_pFence.Get(), Producer.m_NextFenceValue - 1);
	}

	uint64_t CommandQueue::IncrementFence()
	{
		std::lock_guard<std::mutex> LockGuard(m_FenceMutex);
		//tell the command queue to reach the fence value
		m_pCommandQueue->Signal(m_pFence.Get(), m_NextFenceValue);

		return m_NextFenceValue++;
	}

	uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12CommandList> List)
	{
		std::lock_guard<std::mutex> LockGuard(m_FenceMutex);

		PX_CORE_ASSERT(((ID3D12GraphicsCommandList*)List.Get())->Close() >= 0, "Command List Close Error!");

		m_pCommandQueue->ExecuteCommandLists(1, List.GetAddressOf());

		//signal the next fence value
		m_pCommandQueue->Signal(m_pFence.Get(), m_NextFenceValue);

		//increment the fence value
		return m_NextFenceValue++;
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::RequesetAlloactor()
	{
		uint64_t CompletedFence = m_pFence->GetCompletedValue();

		//return a alloactor, that's fence value is less than completed value
		return m_pAlloactorPool.RequestAlloactor(CompletedFence);
	}

	void CommandQueue::DiscardAlloactor(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Alloactor)
	{
		m_pAlloactorPool.DiscardAlloactor(FenceValue, Alloactor);
	}

	//query the fence value, whether the fencevalue is less than the gpu side's fence value
	bool CommandQueue::IsFenceComplete(uint64_t FenceValue)
	{
		if (FenceValue > m_LastCompletedFenceValue)
			m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());

		return FenceValue <= m_LastCompletedFenceValue;
	}

	void CommandQueue::WaitForFence(uint64_t FenceValue)
	{
		if (IsFenceComplete(FenceValue))
			return;

		std::lock_guard<std::mutex> LockGuard(m_EventMutex);

		//when the gpu side's fence value reach to the we have setted fencevalue, then signal fence event handle
		m_pFence->SetEventOnCompletion(FenceValue, m_FenceEventHandle);
		WaitForSingleObject(m_FenceEventHandle, INFINITE);
		m_LastCompletedFenceValue = FenceValue;
	}
	//------Command Queue------

	//------Command List Manager------
	CommandListManager::CommandListManager()
		:m_GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT),
		m_ComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE),
		m_CopyQueue(D3D12_COMMAND_LIST_TYPE_COPY)
	{

	}

	CommandListManager::~CommandListManager()
	{
		ShutDown();
	}

	//Ref<CommandListManager> CommandListManager::Get(Ref<Device> DirectXDevice::Get())
	//{
	//	//return g_pCommandListManager;
	//	if (g_pCommandListManager == nullptr)
	//	{
	//		g_pCommandListManager = std::make_shared<CommandListManager>();
	//		g_pCommandListManager->Create(DirectXDevice::Get());
	//	}
	//	return g_pCommandListManager;
	//}

	void CommandListManager::Create()
	{
		m_GraphicsQueue.Create();
		m_ComputeQueue.Create();
		m_CopyQueue.Create();
	}

	void CommandListManager::ShutDown()
	{
		m_GraphicsQueue.ShutDown();
		m_ComputeQueue.ShutDown();
		m_CopyQueue.ShutDown();
	}

	void CommandListManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& List, 
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& Allocator)
	{
		PX_CORE_ASSERT(Type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Bundles are not yet supported!");

		//return a allocator
		switch (Type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT: Allocator = m_GraphicsQueue.RequesetAlloactor(); break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE: Allocator = m_ComputeQueue.RequesetAlloactor(); break;
		case D3D12_COMMAND_LIST_TYPE_COPY: Allocator = m_CopyQueue.RequesetAlloactor(); break;
		}

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateCommandList(1, Type, Allocator.Get(), nullptr, IID_PPV_ARGS(List.GetAddressOf())) >= 0,
		"Create Command List Error!");
#if PX_DEBUG
		List->SetName(L"CommandListManager::CommandList");
#endif
	}

	CommandQueue& CommandListManager::GetQueue(D3D12_COMMAND_LIST_TYPE Type)
	{
		switch (Type)
		{
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				return m_ComputeQueue;
			case D3D12_COMMAND_LIST_TYPE_COPY:
				return m_CopyQueue;
			default:
				return m_GraphicsQueue;
		}
	}

	bool CommandListManager::IsFenceComplete(uint64_t FenceValue)
	{
		return GetQueue(D3D12_COMMAND_LIST_TYPE(FenceValue >> 56)).IsFenceComplete(FenceValue);
	}

	void CommandListManager::WaitForFence(uint64_t FenceValue)
	{
		CommandQueue& Producer = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->GetQueue((D3D12_COMMAND_LIST_TYPE)(FenceValue >> 56));
		Producer.WaitForFence(FenceValue);
	}

	void CommandListManager::IdleGPU()
	{
		m_GraphicsQueue.WaitForIdle();
		m_ComputeQueue.WaitForIdle();
		m_CopyQueue.WaitForIdle();
	}

	//Ref<CommandListManager> CommandListManager::g_pCommandListManager;

	//------Command List Manager------
}