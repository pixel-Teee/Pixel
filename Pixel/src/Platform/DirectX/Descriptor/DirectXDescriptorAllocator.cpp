#include "pxpch.h"
#include "DirectXDescriptorAllocator.h"

#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
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

	Ref<DescriptorCpuHandle> DirectXDescriptorAllocator::Allocate(uint32_t Count)
	{
		if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
		{
			m_CurrentHeap = RequestNewHeap(m_Type);
			//------important------
			m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
			//------important------
			m_RemainingFreeHandles = sm_NumDescriptorPerHeap;

			m_Offset = 0;

			if (m_DescriptorSize == 0)
				m_DescriptorSize = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->GetDescriptorHandleIncrementSize(m_Type);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
		m_CurrentHandle.ptr += Count * m_DescriptorSize;
		m_RemainingFreeHandles -= Count;
		m_Offset += Count;
		
		Ref<DirectXDescriptorCpuHandle> pCpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		pCpuHandle->SetCpuHandle(ret);
		return pCpuHandle;
	}

	Ref<DescriptorHandle> DirectXDescriptorAllocator::AllocateCpuAndGpuHandle(uint32_t Count)
	{
		if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
		{
			m_CurrentHeap = RequestNewHeap(m_Type);
			//------important------
			m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
			//------important------
			m_RemainingFreeHandles = sm_NumDescriptorPerHeap;
			m_Offset = 0;

			if (m_DescriptorSize == 0)
				m_DescriptorSize = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->GetDescriptorHandleIncrementSize(m_Type);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
		m_CurrentHandle.ptr += Count * m_DescriptorSize;
		m_RemainingFreeHandles -= Count;
		
		Ref<DirectXDescriptorCpuHandle> pCpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		pCpuHandle->SetCpuHandle(ret);

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_CurrentHeap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += m_Offset * m_DescriptorSize;
		
		Ref<DirectXDescriptorGpuHandle> pGpuHandle = std::make_shared<DirectXDescriptorGpuHandle>();
		m_Offset += Count;
		pGpuHandle->SetGpuHandle(gpuHandle);

		Ref<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>(pCpuHandle, pGpuHandle);
		return pHandle;
	}

	void DirectXDescriptorAllocator::DestroyDescriptor()
	{
		sm_DescriptorHeapPool.clear();
	}

	uint32_t DirectXDescriptorAllocator::GetDescriptorSize()
	{
		return m_DescriptorSize;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXDescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type)
	{
		std::lock_guard<std::mutex> LockGuard(sm_AllocationMutex);

		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Desc.Type = Type;
		Desc.NumDescriptors = sm_NumDescriptorPerHeap;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		Desc.NodeMask = 1;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
		//PX_CORE_ASSERT()
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(pHeap.GetAddressOf())) >= 0, "Create Descriptor Heap Error!");
		sm_DescriptorHeapPool.emplace_back(pHeap);
		return pHeap;
	}
	//------Descriptor Allocator------
}