#include "pxpch.h"
#include "DirectXDescriptorAllocator.h"

#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {
	//------Descriptor Allocator------
	//-------static member------
	std::mutex DirectXDescriptorAllocator::sm_AllocationMutex;

	const uint32_t DirectXDescriptorAllocator::sm_NumDescriptorPerHeap;

	//-------static member------
	DirectXDescriptorAllocator::DirectXDescriptorAllocator(DescriptorHeapType Type)
	:m_Type(DescriptorHeapTypeToDirectXDescriptorHeapType(Type)), m_CurrentHeap(nullptr), m_DescriptorSize(0)
	{
		
	}

	Ref<DescriptorCpuHandle> DirectXDescriptorAllocator::Allocate(uint32_t Count, Ref<Device> pDevice)
	{
		if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
		{
			m_CurrentHeap = RequestNewHeap(m_Type, pDevice);
			//------important------
			m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
			//------important------
			m_RemainingFreeHandles = sm_NumDescriptorPerHeap;

			if (m_DescriptorSize == 0)
				m_DescriptorSize = std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->GetDescriptorHandleIncrementSize(m_Type);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
		m_CurrentHandle.ptr += Count * m_DescriptorSize;
		m_RemainingFreeHandles -= Count;
		
		Ref<DirectXDescriptorCpuHandle> pCpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		pCpuHandle->SetCpuHandle(ret);
		return pCpuHandle;
	}

	void DirectXDescriptorAllocator::DestroyDescriptor()
	{
		sm_DescriptorHeapPool.clear();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXDescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, Ref<Device> pDevice)
	{
		std::lock_guard<std::mutex> LockGuard(sm_AllocationMutex);

		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Desc.Type = Type;
		Desc.NumDescriptors = sm_NumDescriptorPerHeap;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Desc.NodeMask = 1;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
		//PX_CORE_ASSERT()
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(pHeap.GetAddressOf())) >= 0, "Create Descriptor Heap Error!");
		sm_DescriptorHeapPool.emplace_back(pHeap);
		return pHeap;
	}
	//------Descriptor Allocator------
}