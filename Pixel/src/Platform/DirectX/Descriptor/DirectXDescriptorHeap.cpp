#include "pxpch.h"

#include "DirectXDescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/TypeUtils.h"

namespace Pixel {

	bool DirectXDescriptorHeap::HasAvailableSpace(uint32_t Count) const
	{
		return Count <= m_NumFreeDescriptors;
	}

	Ref<DescriptorHandle> DirectXDescriptorHeap::Alloc(uint32_t Count /*= 1*/)
	{
		PX_CORE_ASSERT(HasAvailableSpace(Count), "descriptor heap out of space, increase heap size!");

		Ref<DescriptorHandle> Ret = m_NextFreeHandle;
		(*m_NextFreeHandle) += Count * m_DescriptorSize;
		m_NumFreeDescriptors -= Count;
		return Ret;
	}

	DescriptorHandle DirectXDescriptorHeap::operator[](uint32_t arrayIndex) const
	{
		//return std::make_shared<DescriptorHandle>((*m_FirstHandle) + arrayIndex * m_DescriptorSize);
		DescriptorHandle Ret = *m_FirstHandle;
		Ret += arrayIndex * m_DescriptorSize;
		return Ret;
	}

	uint32_t DirectXDescriptorHeap::GetOffsetOfHandle(const DescriptorHandle& handle)
	{
		return (uint32_t)(handle.GetCpuPtr() - m_FirstHandle->GetCpuPtr() / m_DescriptorSize);
	}

	bool DirectXDescriptorHeap::ValidateHandle(const DescriptorHandle& handle) const
	{
		if (handle.GetCpuPtr() < m_FirstHandle->GetCpuPtr() ||
			handle.GetCpuPtr() >= m_FirstHandle->GetCpuPtr() + m_HeapDesc.NumDescriptors * m_DescriptorSize)
			return false;

		if (handle.GetCpuPtr() - m_FirstHandle->GetCpuPtr() !=
			handle.GetCpuPtr() - m_FirstHandle->GetCpuPtr())
			return false;
	}

	ID3D12DescriptorHeap* DirectXDescriptorHeap::GetHeapPointer()
	{
		return m_Heap.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXDescriptorHeap::GetComPtrHeap()
	{
		return m_Heap;
	}

	uint32_t DirectXDescriptorHeap::GetDescriptorSize() const
	{
		return m_DescriptorSize;
	}

	DirectXDescriptorHeap::DirectXDescriptorHeap(const std::wstring& DebugName, DescriptorHeapType Type, uint32_t MaxCount, Ref<Device> pDevice)
	{
		m_HeapDesc.Type = DescriptorHeapTypeToDirectXDescriptorHeapType(Type);
		m_HeapDesc.NumDescriptors = MaxCount;
		m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_HeapDesc.NodeMask = 1;


		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_Heap.GetAddressOf())) >= 0,
			"Create Descriptor Heap Error!");

#ifdef PX_DEBUG
		m_Heap->SetName(DebugName.c_str());
#else
		(void)DebugHeapName;
#endif

		m_DescriptorSize = std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
		m_NumFreeDescriptors = m_HeapDesc.NumDescriptors;

		Ref<DirectXDescriptorCpuHandle> cpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		cpuHandle->SetCpuHandle(m_Heap->GetCPUDescriptorHandleForHeapStart());
 
		Ref<DirectXDescriptorGpuHandle> gpuHandle = std::make_shared<DirectXDescriptorGpuHandle>();
		gpuHandle->SetGpuHandle(m_Heap->GetGPUDescriptorHandleForHeapStart());

		m_FirstHandle = std::make_shared<DescriptorHandle>(cpuHandle, gpuHandle);
		m_NextFreeHandle = m_FirstHandle;
	}

	DirectXDescriptorHeap::DirectXDescriptorHeap()
	{

	}

}