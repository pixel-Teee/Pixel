#include "pxpch.h"

#include "DirectXStructuredBuffer.h"

#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	StructuredBuffer::~StructuredBuffer()
	{
		Destroy();
	}

	void StructuredBuffer::CreateDerivedViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.NumElements = m_ElementCount;
		SRVDesc.Buffer.StructureByteStride = m_ElementSize;
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		if (m_SRV->GetCpuPtr() == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
			m_SRV = DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1);
		
		Ref<DirectXDescriptorCpuHandle> pSrv = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRV);
		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateShaderResourceView(m_pResource.Get(), &SRVDesc, pSrv->GetCpuHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		UAVDesc.Buffer.CounterOffsetInBytes = 0;
		UAVDesc.Buffer.NumElements = m_ElementCount;
		UAVDesc.Buffer.StructureByteStride = m_ElementSize;
		UAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		if (m_UAV->GetCpuPtr() == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
			m_UAV = DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAV)->GetCpuHandle();
		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateUnorderedAccessView(m_pResource.Get(), nullptr, &UAVDesc, handle);
	}

}