#include "pxpch.h"

#include "ColorBuffer.h"

#include "DescriptorAllocator.h"
#include "DirectXDevice.h"

namespace Pixel {

	ColorBuffer::ColorBuffer(glm::vec4 ClearColor /*= glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)*/)
		:m_ClearColor(ClearColor), m_NumMipMaps(0), m_FragmentCount(1), m_SampleCount(1)
	{
		m_RTVHandle.ptr = -1;//D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
		m_SRVHandle.ptr = -1;
		for (size_t i = 0; i < _countof(m_UAVHandle); ++i)
			m_UAVHandle[i].ptr = -1;
	}

	void ColorBuffer::CreateFromSwapChain(const std::wstring& Name, ID3D12Resource* pBaseResouce)
	{
		AssociateWithResource(Name, pBaseResouce, D3D12_RESOURCE_STATE_PRESENT);

		m_RTVHandle = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		DirectXDevice::Get()->GetDevice()->CreateRenderTargetView(m_pResource.Get(), nullptr, m_RTVHandle);
	}

	void ColorBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, 
	uint32_t NumMips, DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VideoMemoryPtr /*= -1*/)
	{
		NumMips = (NumMips == 0 ? ComputeNumMips(Width, Height) : NumMips);

		D3D12_RESOURCE_FLAGS Flags = CombineResourceFlags();
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, 1, NumMips, Format, Flags);

		//the number of multisamples per pixel
		ResourceDesc.SampleDesc.Count = m_FragmentCount;
		ResourceDesc.SampleDesc.Quality = 0;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = Format;
		ClearValue.Color[0] = m_ClearColor.r;
		ClearValue.Color[1] = m_ClearColor.g;
		ClearValue.Color[2] = m_ClearColor.b;
		ClearValue.Color[3] = m_ClearColor.a;

		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr);
		CreateDerivedViews(Format, 1, NumMips);
	}

	//void ColorBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips, DXGI_FORMAT Format)
	//{
	//	Create(Name, Width, Height, NumMips, Format);
	//}

	void ColorBuffer::CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
	DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VideoMemoryPtr /*= -1*/)
	{
		D3D12_RESOURCE_FLAGS Flags = CombineResourceFlags();
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, ArrayCount, 1, Format, Flags);

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = Format;
		ClearValue.Color[0] = m_ClearColor.r;
		ClearValue.Color[1] = m_ClearColor.g;
		ClearValue.Color[2] = m_ClearColor.b;
		ClearValue.Color[3] = m_ClearColor.a;

		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr);
		CreateDerivedViews(Format, ArrayCount, 1);
	}

	void ColorBuffer::SetMsaaMode(uint32_t NumColorSamples, uint32_t NumConverageSamples)
	{
		PX_CORE_ASSERT(NumConverageSamples >= NumColorSamples, "converage samplers need don't exceed color samplers!");

		m_FragmentCount = NumColorSamples;
		m_SampleCount = NumConverageSamples;
	}

	//need to implement
	void ColorBuffer::GenerateMipMaps(DirectXContext& context)
	{
		if (m_NumMipMaps == 0)
			return;

		

	}

	//void ColorBuffer::CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount, DXGI_FORMAT Format)
	//{
	//	Cre
	//}

	D3D12_RESOURCE_FLAGS ColorBuffer::CombineResourceFlags() const
	{
		D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

		if (Flags == D3D12_RESOURCE_FLAG_NONE && m_FragmentCount == 1)
			Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
	}

	uint32_t ColorBuffer::ComputeNumMips(uint32_t Width, uint32_t Height)
	{
		uint32_t HightBit;
		_BitScanReverse((unsigned long*)&HightBit, Width | Height);
		return HightBit + 1;
	}

	void ColorBuffer::CreateDerivedViews(DXGI_FORMAT Format, uint32_t ArraySize, uint32_t NumMips /*= 1*/)
	{
		PX_CORE_ASSERT(ArraySize == 1 || NumMips == 1, "we don't support auto-mips on texture arrays");

		m_NumMipMaps = NumMips - 1;

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

		RTVDesc.Format = Format;
		UAVDesc.Format = GetUAVFormat(Format);
		SRVDesc.Format = Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (ArraySize > 1)
		{
			RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.MipSlice = 0;
			RTVDesc.Texture2DArray.FirstArraySlice = 0;
			RTVDesc.Texture2DArray.ArraySize = (uint32_t)ArraySize;

			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			UAVDesc.Texture2DArray.MipSlice = 0;
			UAVDesc.Texture2DArray.FirstArraySlice = 0;
			UAVDesc.Texture2DArray.ArraySize = (uint32_t)ArraySize;

			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			SRVDesc.Texture2DArray.MipLevels = NumMips;
			SRVDesc.Texture2DArray.MostDetailedMip = 0;
			SRVDesc.Texture2DArray.FirstArraySlice = 0;
			SRVDesc.Texture2DArray.ArraySize = (uint32_t)ArraySize;
		}
		else if (m_FragmentCount > 1)
		{
			RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			RTVDesc.Texture2D.MipSlice = 0;

			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			UAVDesc.Texture2D.MipSlice = 0;

			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = NumMips;
			SRVDesc.Texture2D.MostDetailedMip = 0;
		}

		if (m_SRVHandle.ptr == -1)
		{
			m_RTVHandle = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_SRVHandle = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		ID3D12Resource* pResource = m_pResource.Get();

		//create the render target view
		DirectXDevice::Get()->GetDevice()->CreateRenderTargetView(pResource, &RTVDesc, m_RTVHandle);

		//create the shader resource view
		DirectXDevice::Get()->GetDevice()->CreateShaderResourceView(pResource, &SRVDesc, m_SRVHandle);

		if (m_FragmentCount > 1) return;

		for (uint32_t i = 0; i < NumMips; ++i)
		{
			if (m_UAVHandle[i].ptr == -1)
				m_UAVHandle[i] = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			DirectXDevice::Get()->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &UAVDesc, m_UAVHandle[i]);

			++UAVDesc.Texture2D.MipSlice;
		}
	}

}