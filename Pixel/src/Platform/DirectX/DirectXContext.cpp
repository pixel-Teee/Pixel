#include "pxpch.h"

#include "DirectXContext.h"
#include "CommandQueue.h"
#include "DirectXSwapChain.h"
#include "DescriptorAllocator.h"
#include "DirectXGpuBuffer.h"
#include "DirectXGpuResource.h"
#include "GraphicsContext.h"
#include "PipelineStateObject.h"
#include "Pixel/Math/Math.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace Pixel {

	//------Context Manager------
	ContextManager g_ContextManager;

	ContextManager::ContextManager()
	{

	}

	Ref<DirectXContext> ContextManager::AllocateContext(D3D12_COMMAND_LIST_TYPE Type)
	{
		std::lock_guard<std::mutex> lockGuard(sm_ContextAllocationMutex);

		auto& AvailableContexts = sm_AvailableContexts[Type];

		Ref<DirectXContext> returnContext;
		if (AvailableContexts.empty())
		{
			returnContext = CreateRef<DirectXContext>(Type);
			sm_ContextPool[Type].emplace_back(returnContext);
			returnContext->Initialize();
		}
		else
		{
			returnContext = AvailableContexts.front();
			AvailableContexts.pop();
			returnContext->Reset();
		}

		return returnContext;
	}

	void ContextManager::FreeContext(Ref<DirectXContext> UsedContext)
	{
		std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);
		sm_AvailableContexts[UsedContext->m_Type].push(UsedContext);
	}

	void ContextManager::DestroyAllContexts()
	{
		for (uint32_t i = 0; i < 4; ++i)
			sm_ContextPool[i].clear();
	}
	//------Context Manager------

//------a marco------
#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
    ( D3D12_RESOURCE_STATE_UNORDERED_ACCESS \
    | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE \
    | D3D12_RESOURCE_STATE_COPY_DEST \
    | D3D12_RESOURCE_STATE_COPY_SOURCE )
//-------a marco------

	//-------DirectX Context------
	DirectXContext::DirectXContext(D3D12_COMMAND_LIST_TYPE Type)
		:m_Type(Type),
		m_CpuLinearAllocator(kCpuWritable),
		m_GpuLinearAllocator(kGpuExclusive),
		m_DynamicViewDescriptorHeap(*this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
		m_DynamicSamplerDescriptorHeap(*this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		
	}

	DirectXContext::~DirectXContext()
	{
	}

	DirectXContext& DirectXContext::Begin(const std::wstring ID)
	{
		//request a new context from the context pool
		Ref<DirectXContext> NewContext = g_ContextManager.AllocateContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		NewContext->SetID(ID);

		return *NewContext.get();
	}

	void DirectXContext::DestroyAllContexts()
	{
		g_ContextManager.DestroyAllContexts();
	}

	void DirectXContext::Initialize()
	{
		CommandListManager::Get()->CreateNewCommandList(m_Type, m_pCommandList, m_pCurrentAllocator);
	}

	void DirectXContext::SwapBuffers()
	{
	
	}

	uint64_t DirectXContext::Flush(bool WaitForCompletion)
	{
		FlushResourceBarriers();

		uint64_t FenceValue = CommandListManager::Get()->GetQueue(m_Type).ExecuteCommandList(m_pCommandList);

		if (WaitForCompletion)
			CommandListManager::Get()->WaitForFence(FenceValue);

		//reset command list and restore previous state
		m_pCommandList->Reset(m_pCurrentAllocator.Get(), nullptr);

		if (m_CurrGraphicsRootSignature)
		{
			m_pCommandList->SetGraphicsRootSignature(m_CurrGraphicsRootSignature.Get());
		}

		if (m_CurrComputeRootSingature)
		{
			m_pCommandList->SetComputeRootSignature(m_CurrComputeRootSingature.Get());
		}

		if (m_CurrPipelineState)
		{
			m_pCommandList->SetPipelineState(m_CurrPipelineState.Get());
		}

		//Bind Descriptors
		BindDescriptorHeaps();

		return FenceValue;
	}

	uint64_t DirectXContext::Finish(bool WaitForCompletion)
	{
		FlushResourceBarriers();

		CommandQueue& Queue = CommandListManager::Get()->GetQueue(m_Type);

		uint64_t FenceValue = Queue.ExecuteCommandList(m_pCommandList);
		Queue.DiscardAlloactor(FenceValue, m_pCurrentAllocator);
		//release and call the instance's release function
		m_pCurrentAllocator->Reset();

		if (WaitForCompletion)
			CommandListManager::Get()->WaitForFence(FenceValue);

		g_ContextManager.FreeContext(shared_from_this());

		return FenceValue;
	}

	void DirectXContext::FlushResourceBarriers()
	{
		if (m_NumBarriersToFlush > 0)
		{
			//transition resource barrier
			m_pCommandList->ResourceBarrier(m_NumBarriersToFlush, m_ResourceBarrierBuffer);
			m_NumBarriersToFlush = 0;
		}
	}

	void DirectXContext::BindDescriptorHeaps()
	{
		uint32_t NonNullHeaps = 0;

		ID3D12DescriptorHeap* HeapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			ID3D12DescriptorHeap* HeapIter = m_CurrentDescriptorHeaps[i].Get();
			if (HeapIter != nullptr)
				HeapsToBind[NonNullHeaps++] = HeapIter;
		}

		if (NonNullHeaps > 0)
			m_pCommandList->SetDescriptorHeaps(NonNullHeaps, HeapsToBind);
	}

	void DirectXContext::CopyBuffer(DirectXGpuResource& Dest, DirectXGpuResource& Src)
	{
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarriers();

		//------Copy Resource------
		m_pCommandList->CopyResource(Dest.GetResource(), Src.GetResource());
		//------Copy Resource------
	}

	void DirectXContext::CopyBufferRegion(DirectXGpuResource& Dest, size_t DestOffset, DirectXGpuResource& Src, size_t SrcOffset, size_t NumBytes)
	{
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarriers();

		m_pCommandList->CopyBufferRegion(Dest.GetResource(), DestOffset, Src.GetResource(), SrcOffset, NumBytes);
	}

	void DirectXContext::CopySubresource(DirectXGpuResource& Dest, uint32_t DestSubIndex, DirectXGpuResource& Src, uint32_t SrcSubIndex)
	{
		FlushResourceBarriers();

		D3D12_TEXTURE_COPY_LOCATION DestLocation =
		{
			Dest.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			DestSubIndex
		};

		//subresource index:specifies the index of the subresouce of an arrayed, mip-mapped, or planar texture should be used fo the copy operation
		D3D12_TEXTURE_COPY_LOCATION SrcLocation =
		{
			Src.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			SrcSubIndex
		};

		//use gpu to copy texture data between two locations
		m_pCommandList->CopyTextureRegion(&DestLocation, 0, 0, 0, &SrcLocation, nullptr);
	}

	void DirectXContext::CopyTextureRegion(DirectXGpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, DirectXGpuResource& Source, RECT& Rect)
	{
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Source, D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarriers();

		//0:subresource index
		D3D12_TEXTURE_COPY_LOCATION destLoc = CD3DX12_TEXTURE_COPY_LOCATION(Dest.GetResource(), 0);
		D3D12_TEXTURE_COPY_LOCATION srcLoc = CD3DX12_TEXTURE_COPY_LOCATION(Source.GetResource(), 0);

		D3D12_BOX box = {};
		box.back = 1;//z position of box
		box.left = Rect.left;
		box.right = Rect.right;
		box.top = Rect.top;
		box.bottom = Rect.bottom;

		m_pCommandList->CopyTextureRegion(&destLoc, x, y, z, &srcLoc, &box);
	}

	void DirectXContext::TransitionResource(DirectXGpuResource& Resource, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
	{
		D3D12_RESOURCE_STATES OldState = Resource.m_UsageState;

		if (m_Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			PX_CORE_ASSERT((OldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == OldState, "resource state is not valid!");
			PX_CORE_ASSERT((OldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == NewState, "resource state is not valid!");
		}

		if (OldState != NewState)
		{
			PX_CORE_ASSERT(m_NumBarriersToFlush < 16, "exceeded arbitrary limit on buffered barriers!");
			//add a new barrier
			D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

			BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			BarrierDesc.Transition.pResource = Resource.GetResource();
			BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			BarrierDesc.Transition.StateBefore = OldState;
			BarrierDesc.Transition.StateAfter = NewState;

			//check to see if we already started the transition
			if (NewState == Resource.m_TransitioningState)
			{
				BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
				Resource.m_TransitioningState = (D3D12_RESOURCE_STATES)-1;
			}
			else
				BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			Resource.m_UsageState = NewState;
		}//oldstate == newstate && newstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		else if (NewState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			InsertUAVBarrier(Resource, FlushImmediate);
		}

		if (FlushImmediate || m_NumBarriersToFlush == 16)
			FlushResourceBarriers();
	}

	void DirectXContext::InsertUAVBarrier(DirectXGpuResource& Resource, bool FlushImmediate)
	{
		PX_CORE_ASSERT(m_NumBarriersToFlush < 16, "exceeded arbitrary limit on buffered barriers");
		D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		BarrierDesc.UAV.pResource = Resource.GetResource();

		if (FlushImmediate)
			FlushResourceBarriers();
	}

	void DirectXContext::InitializeTexture(DirectXGpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[])
	{
		//returns the required size of a buffer to be used for data upload
		uint64_t uploadBufferSize = GetRequiredIntermediateSize(Dest.GetResource(), 0, NumSubresources);

		DirectXContext& InitContext = DirectXContext::Begin();

		//copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
		DynAlloc mem = InitContext.ReserveUploadMemory(uploadBufferSize);
		UpdateSubresources(InitContext.m_pCommandList.Get(), Dest.GetResource(), mem.Buffer.GetResource(), 0, 0, NumSubresources, SubData);
		InitContext.TransitionResource(Dest, D3D12_RESOURCE_STATE_GENERIC_READ);
		//execute the command list and wait for it to finish so we can release the upload buffer
		//finish can release upload buffer
		InitContext.Finish(true);
	}

	void DirectXContext::InitializeBuffer(DirectXGpuBuffer& Dest, const void* BufferData, size_t NumBytes, size_t DestOffset /*= 0*/)
	{
		DirectXContext& InitContext = DirectXContext::Begin();

		DynAlloc mem = InitContext.ReserveUploadMemory(NumBytes);
		memcpy(mem.DataPtr, BufferData, Math::DivideByMultiple(NumBytes, 16) * 16);
		
		//copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
		InitContext.TransitionResource(Dest.m_GpuResource, D3D12_RESOURCE_STATE_COPY_DEST, true);
		InitContext.m_pCommandList->CopyBufferRegion(Dest.m_GpuResource.m_pResource.Get(), DestOffset, mem.Buffer.GetResource(), 0, NumBytes);
		InitContext.TransitionResource(Dest.m_GpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, true);

		//execute the command list and wait for it to finish so we can release the upload buffer
		InitContext.Finish(true);
	}

	void DirectXContext::InitializeTextureArraySlice(DirectXGpuResource& Dest, uint32_t SliceIndex, DirectXGpuResource& Src)
	{
		DirectXContext& Context = DirectXContext::Begin();

		Context.TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		Context.FlushResourceBarriers();

		const D3D12_RESOURCE_DESC& DestDesc = Dest.GetResource()->GetDesc();
		const D3D12_RESOURCE_DESC& SrcDesc = Src.GetResource()->GetDesc();

		PX_CORE_ASSERT(SliceIndex < DestDesc.DepthOrArraySize&&
			SrcDesc.DepthOrArraySize == 1 &&
			DestDesc.Width == SrcDesc.Width &&
			DestDesc.Height == SrcDesc.Height &&
			DestDesc.MipLevels <= SrcDesc.MipLevels, "slice index is error!"
		);

		uint32_t SubResourceIndex = SliceIndex * DestDesc.MipLevels;

		for (uint32_t i = 0; i < DestDesc.MipLevels; ++i)
		{
			D3D12_TEXTURE_COPY_LOCATION destCopyLocation = {
				Dest.GetResource(),
				D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				SubResourceIndex + i
			};

			D3D12_TEXTURE_COPY_LOCATION srcCopyLocation =
			{
				Src.GetResource(),
				D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				i
			};

			Context.m_pCommandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);
		};

		Context.TransitionResource(Dest, D3D12_RESOURCE_STATE_GENERIC_READ);

		Context.Finish(true);
	}

	void DirectXContext::WriteBuffer(DirectXGpuResource& Dest, size_t DestOffset, const void* BufferData, size_t NumBytes)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is not aligned!");

		DynAlloc TempSpace = m_CpuLinearAllocator.Allocate(NumBytes, 512);
		memcpy(TempSpace.DataPtr, BufferData, Math::DivideByMultiple(NumBytes, 16) * 16);
		CopyBufferRegion(Dest, DestOffset, TempSpace.Buffer, TempSpace.Offset, NumBytes);
	}

	DynAlloc DirectXContext::ReserveUploadMemory(size_t SizeInBytes)
	{
		return m_CpuLinearAllocator.Allocate(SizeInBytes);
	}

	void DirectXContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr)
	{
		if (m_CurrentDescriptorHeaps[Type] != HeapPtr)
		{
			m_CurrentDescriptorHeaps[Type] = HeapPtr;
			BindDescriptorHeaps();
		}
	}

	void DirectXContext::SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[])
	{
		bool AnyChanged = false;
		for (uint32_t i = 0; i < HeapCount; ++i)
		{
			if (m_CurrentDescriptorHeaps[Type[i]] != HeapPtrs[i])
			{
				m_CurrentDescriptorHeaps[Type[i]] = HeapPtrs[i];
				AnyChanged = true;
			}
		}

		if (AnyChanged)
			BindDescriptorHeaps();
	}

	void DirectXContext::SetPipelineState(const PSO& pso)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState = pso.GetPipelineStateObject();

		if (PipelineState == m_CurrPipelineState)
			return;

		m_pCommandList->SetPipelineState(PipelineState.Get());
		m_CurrPipelineState = PipelineState;
	}

	GraphicsContext& DirectXContext::GetGraphicsContext()
	{
		PX_CORE_ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_COMPUTE, "cannot convert async compute context to graphics!");
		return static_cast<GraphicsContext&>(*this);
	}

	void DirectXContext::Reset()
	{
		m_pCurrentAllocator = CommandListManager::Get()->GetQueue(m_Type).RequesetAlloactor();
		m_pCommandList->Reset(m_pCurrentAllocator.Get(), nullptr);

		m_CurrGraphicsRootSignature = nullptr;
		m_CurrComputeRootSingature = nullptr;
		m_CurrPipelineState = nullptr;
		m_NumBarriersToFlush = 0;

		BindDescriptorHeaps();
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DirectXContext::GetCommandList()
	{
		return m_pCommandList;
	}

	//-------DirectX Context------
}