#include "pxpch.h"
#include "DescriptorAllocator.h"

#include "DirectXDevice.h"

namespace Pixel {
	//------Descriptor Allocator------
	//-------static member------
	std::mutex DescriptorAllocator::sm_AllocationMutex;

	std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::sm_DescriptorHeapPool;

	const uint32_t DescriptorAllocator::sm_NumDescriptorPerHeap;

	DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	};
	//-------static member------
	DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE Type)
	:m_Type(Type), m_CurrentHeap(nullptr), m_DescriptorSize(0)
	{
		
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(uint32_t Count)
	{
		if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
		{
			m_CurrentHeap = RequestNewHeap(m_Type);
			m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
			m_RemainingFreeHandles = sm_NumDescriptorPerHeap;

			if (m_DescriptorSize == 0)
				m_DescriptorSize = DirectXDevice::Get()->GetDevice()->GetDescriptorHandleIncrementSize(m_Type);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
		m_CurrentHandle.ptr += Count * m_DescriptorSize;
		m_RemainingFreeHandles -= Count;
		return ret;
	}

	void DescriptorAllocator::DestroyAll(void)
	{
		sm_DescriptorHeapPool.clear();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t Count /*= 1*/)
	{
		return g_DescriptorAllocator[Type].Allocate(Count);
	}

	ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type)
	{
		std::lock_guard<std::mutex> LockGuard(sm_AllocationMutex);

		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Desc.Type = Type;
		Desc.NumDescriptors = sm_NumDescriptorPerHeap;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Desc.NodeMask = 1;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
		//PX_CORE_ASSERT()
		PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap)), "Create Descriptor Heap Error!");
		sm_DescriptorHeapPool.emplace_back(pHeap);
		return pHeap.Get();
	}
	//------Descriptor Allocator------

	//------Descriptor Heap------
	DescriptorHeap::DescriptorHeap()
	{

	}

	DescriptorHeap::~DescriptorHeap()
	{
		Destroy();
	}

	void DescriptorHeap::Create(const std::wstring& DebugHeapName, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t MaxCount)
	{
		m_HeapDesc.Type = Type;
		m_HeapDesc.NumDescriptors = MaxCount;
		m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_HeapDesc.NodeMask = 1;

		PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_Heap.ReleaseAndGetAddressOf())) >= 0,
			"Create Descriptor Heap Error!");

#ifdef PX_DEBUG
		m_Heap->SetName(DebugHeapName.c_str());
#else
		(void)DebugHeapName;
#endif

		m_DescriptorSize = DirectXDevice::Get()->GetDevice()->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
		m_NumFreeDescriptors = m_HeapDesc.NumDescriptors;

		Ref<DirectXDescriptorCpuHandle> cpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		cpuHandle->SetCpuHandle(m_Heap->GetCPUDescriptorHandleForHeapStart());

		Ref<DirectXDescriptorGpuHandle> gpuHandle = std::make_shared<DirectXDescriptorGpuHandle>();
		gpuHandle->SetGpuHandle(m_Heap->GetGPUDescriptorHandleForHeapStart());

		m_FirstHandle = DescriptorHandle(
			cpuHandle,
			gpuHandle
		);
		m_NextFreeHandle = m_FirstHandle;
	}

	void DescriptorHeap::Destroy()
	{
		m_Heap = nullptr;
	}

	bool DescriptorHeap::HasAvailableSpace(uint32_t Count) const
	{
		return Count <= m_NumFreeDescriptors;
	}

	DescriptorHandle DescriptorHeap::Alloc(uint32_t Count)
	{
		PX_CORE_ASSERT(HasAvailableSpace(Count), "Descriptor heap out of space.");
		DescriptorHandle ret = m_NextFreeHandle;
		m_NextFreeHandle += Count * m_DescriptorSize;
		m_NumFreeDescriptors -= Count;
		return ret;
	}

	uint32_t DescriptorHeap::GetOffsetOfHandle(const DirectXDescriptorCpuHandle& DHandle)
	{
		return (uint32_t)(DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr()) / m_DescriptorSize;
	}

	bool DescriptorHeap::ValidateHandle(const DescriptorHandle& DHandle) const
	{
		if (DHandle.GetCpuPtr() < m_FirstHandle.GetCpuPtr() ||
			DHandle.GetCpuPtr() >= m_FirstHandle.GetCpuPtr() + m_HeapDesc.NumDescriptors * m_DescriptorSize)
			return false;

		if (DHandle.GetGpuPtr() - m_FirstHandle.GetGpuPtr() !=
			DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr())
			return false;

		return true;
	}

	//------Descriptor Heap------
}