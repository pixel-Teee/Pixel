#include "pxpch.h"

#include "DepthBuffer.h"

#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"

namespace Pixel {

	DepthBuffer::DepthBuffer(float ClearDepth /*= 0.0f*/, uint8_t ClearStencil /*= 0*/)
		:m_ClearDepth(ClearDepth),
		m_ClearStencil(ClearStencil)
	{
		m_hDSV[0] = CreateRef<DirectXDescriptorCpuHandle>();
		m_hDSV[1] = CreateRef<DirectXDescriptorCpuHandle>();
		m_hDSV[2] = CreateRef<DirectXDescriptorCpuHandle>();
		m_hDSV[3] = CreateRef<DirectXDescriptorCpuHandle>();
		m_hDepthSRV = CreateRef<DirectXDescriptorCpuHandle>();
		m_hStencilSRV = CreateRef<DirectXDescriptorCpuHandle>();

		D3D12_CPU_DESCRIPTOR_HANDLE tempHandle;
		tempHandle.ptr = -1;

		for (uint32_t i = 0; i < 4; ++i) 
			m_hDSV[i]->SetCpuHandle(tempHandle);

		m_hDepthSRV->SetCpuHandle(tempHandle);
		m_hStencilSRV->SetCpuHandle(tempHandle);
	}

	void DepthBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height,
		ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/)
	{
		Create(Name, Width, Height, 1, Format, VideoMemoryPtr);
	}

	void DepthBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, 
		uint32_t NumSamples, ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/)
	{
		//m_pResource = CreateRef<DirectXGpuResource>();
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, 1, 1, FormatToDXGIFormat(Format), D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ResourceDesc.SampleDesc.Count = NumSamples;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = FormatToDXGIFormat(Format);
		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr);
		CreateDerivedViews(FormatToDXGIFormat(Format));
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetDSV()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hDSV[0]);
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetDSV_DepthReadOnly()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hDSV[1]);
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetDSV_StencilReadOnly()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hDSV[2]);
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetDSV_ReadOnly()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hDSV[3]);
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetDepthSRV()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hDepthSRV);
	}

	Ref<DescriptorCpuHandle> DepthBuffer::GetStencilSRV()
	{
		return std::static_pointer_cast<DescriptorCpuHandle>(m_hStencilSRV);
	}

	void DepthBuffer::CreateDerivedViews(DXGI_FORMAT Format)
	{
		ID3D12Resource* pResource = m_pResource.Get();

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = GetDSVFormat(Format);
		if (pResource->GetDesc().SampleDesc.Count == 1)
		{
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			//multiple sampler count
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}

		if (m_hDSV[0]->GetCpuHandle().ptr == -1)
		{
			m_hDSV[0] = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::DSV, 1));
			m_hDSV[1] = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::DSV, 1));
		}

		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[0]->GetCpuHandle());

		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[1]->GetCpuHandle());

		DXGI_FORMAT StencilReadFormat = GetStencilFormat(Format);
		if (StencilReadFormat != DXGI_FORMAT_UNKNOWN)
		{
			if (m_hDSV[2]->GetCpuHandle().ptr == -1)
			{
				m_hDSV[2] = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::DSV, 1));
				m_hDSV[3] = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::DSV, 1));
			}

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[2]->GetCpuHandle());

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[3]->GetCpuHandle());
		}
		else
		{
			m_hDSV[2] = m_hDSV[0];
			m_hDSV[3] = m_hDSV[1];
		}

		if (m_hDepthSRV->GetCpuHandle().ptr == -1)
			m_hDepthSRV = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1));

		//create the shader resource view
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = GetDepthFormat(Format);

		if (dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		}

		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateShaderResourceView(pResource, &SRVDesc, m_hDepthSRV->GetCpuHandle());
	}

}

