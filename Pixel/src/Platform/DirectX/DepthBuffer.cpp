#include "pxpch.h"

#include "DepthBuffer.h"

#include "DescriptorAllocator.h"
#include "DirectXDevice.h"

namespace Pixel {

	DepthBuffer::DepthBuffer(float ClearDepth /*= 0.0f*/, uint8_t ClearStencil /*= 0*/)
		:m_ClearDepth(ClearDepth),
		m_ClearStencil(ClearStencil)
	{
		m_hDSV[0].ptr = -1;
		m_hDSV[1].ptr = -1;
		m_hDSV[2].ptr = -1;
		m_hDSV[3].ptr = -1;
		m_hDepthSRV.ptr = -1;
		m_hStencilSRV.ptr = -1;
	}

	void DepthBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height,
		ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/)
	{
		Create(Name, Width, Height, 1, Format, VideoMemoryPtr);
	}

	void DepthBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, 
		uint32_t NumSamples, ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/)
	{
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, 1, 1, FormatToDXGIFormat(Format), D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ResourceDesc.SampleDesc.Count = NumSamples;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = FormatToDXGIFormat(Format);
		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr);
		CreateDerivedViews(FormatToDXGIFormat(Format));
	}

	void DepthBuffer::CreateDerivedViews(DXGI_FORMAT Format)
	{
		ID3D12Resource* pResource = m_GpuResource.m_pResource.Get();

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

		if (m_hDSV[0].ptr == -1)
		{
			m_hDSV[0] = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_hDSV[1] = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		}

		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		DirectXDevice::Get()->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[0]);

		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		DirectXDevice::Get()->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[1]);

		DXGI_FORMAT StencilReadFormat = GetStencilFormat(Format);
		if (StencilReadFormat != DXGI_FORMAT_UNKNOWN)
		{
			if (m_hDSV[2].ptr == -1)
			{
				m_hDSV[2] = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				m_hDSV[3] = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			}

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			DirectXDevice::Get()->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[2]);

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			DirectXDevice::Get()->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, m_hDSV[3]);
		}
		else
		{
			m_hDSV[2] = m_hDSV[0];
			m_hDSV[3] = m_hDSV[1];
		}

		if (m_hDepthSRV.ptr == -1)
			m_hDepthSRV = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
		DirectXDevice::Get()->GetDevice()->CreateShaderResourceView(pResource, &SRVDesc, m_hDepthSRV);
	}

}

