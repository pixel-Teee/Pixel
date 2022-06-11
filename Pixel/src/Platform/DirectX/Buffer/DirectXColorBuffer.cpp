#include "pxpch.h"

#include "DirectXColorBuffer.h"

#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	DirectXColorBuffer::DirectXColorBuffer(glm::vec4 ClearColor /*= glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)*/)
		:m_ClearColor(ClearColor), m_NumMipMaps(0), m_FragmentCount(1), m_SampleCount(1)
	{
		m_RTVHandle = DescriptorCpuHandle::Create();
		m_SRVHandle = DescriptorCpuHandle::Create();
		Ref<DirectXDescriptorCpuHandle> rtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_RTVHandle);
		Ref<DirectXDescriptorCpuHandle> srvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRVHandle);

		D3D12_CPU_DESCRIPTOR_HANDLE tempHandle;
		tempHandle.ptr = -1;

		rtvHandle->SetCpuHandle(tempHandle);//D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
		srvHandle->SetCpuHandle(tempHandle);
		for (size_t i = 0; i < _countof(m_UAVHandle); ++i)
		{
			m_UAVHandle[i] = DescriptorCpuHandle::Create();
			Ref<DirectXDescriptorCpuHandle> uavHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAVHandle[i]);
			uavHandle->SetCpuHandle(tempHandle);
		}
	}

	void DirectXColorBuffer::CreateFromSwapChain(const std::wstring& Name, Ref<Device> pDevice)
	{
		//AssociateWithResource(Name, static_cast<DirectXGpuResource*>(pBaseResouce)->GetResource(), D3D12_RESOURCE_STATE_PRESENT);

		Ref<DirectXDescriptorCpuHandle> rtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_RTVHandle);

		rtvHandle->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::RTV, 1, pDevice))->GetCpuHandle());

		std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateRenderTargetView(std::static_pointer_cast<DirectXGpuResource>(m_pResource)->GetResource(), nullptr, rtvHandle->GetCpuHandle());
	}

	void DirectXColorBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, 
	uint32_t NumMips, ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/, Ref<Device> pDevice)
	{
		NumMips = (NumMips == 0 ? ComputeNumMips(Width, Height) : NumMips);

		D3D12_RESOURCE_FLAGS Flags = CombineResourceFlags();
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, 1, NumMips, FormatToDXGIFormat(Format), Flags);

		//the number of multisamples per pixel
		ResourceDesc.SampleDesc.Count = m_FragmentCount;
		ResourceDesc.SampleDesc.Quality = 0;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = FormatToDXGIFormat(Format);
		ClearValue.Color[0] = m_ClearColor.r;
		ClearValue.Color[1] = m_ClearColor.g;
		ClearValue.Color[2] = m_ClearColor.b;
		ClearValue.Color[3] = m_ClearColor.a;

		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr, pDevice);
		CreateDerivedViews(Format, 1, NumMips, pDevice);
	}

	//void ColorBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips, DXGI_FORMAT Format)
	//{
	//	Create(Name, Width, Height, NumMips, Format);
	//}

	void DirectXColorBuffer::CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
	ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/, Ref<Device> pDevice)
	{
		D3D12_RESOURCE_FLAGS Flags = CombineResourceFlags();
		D3D12_RESOURCE_DESC ResourceDesc = DescribeTex2D(Width, Height, ArrayCount, 1, FormatToDXGIFormat(Format), Flags);

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = FormatToDXGIFormat(Format);
		ClearValue.Color[0] = m_ClearColor.r;
		ClearValue.Color[1] = m_ClearColor.g;
		ClearValue.Color[2] = m_ClearColor.b;
		ClearValue.Color[3] = m_ClearColor.a;

		CreateTextureResource(Name, ResourceDesc, ClearValue, VideoMemoryPtr, pDevice);
		CreateDerivedViews(Format, ArrayCount, 1, pDevice);
	}

	void DirectXColorBuffer::SetMsaaMode(uint32_t NumColorSamples, uint32_t NumConverageSamples)
	{
		PX_CORE_ASSERT(NumConverageSamples >= NumColorSamples, "converage samplers need don't exceed color samplers!");

		m_FragmentCount = NumColorSamples;
		m_SampleCount = NumConverageSamples;
	}

	//need to implement
	void DirectXColorBuffer::GenerateMipMaps(DirectXContext& context)
	{
		if (m_NumMipMaps == 0)
			return;

		

	}

	//void ColorBuffer::CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount, DXGI_FORMAT Format)
	//{
	//	Cre
	//}

	D3D12_RESOURCE_FLAGS DirectXColorBuffer::CombineResourceFlags() const
	{
		D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

		if (Flags == D3D12_RESOURCE_FLAG_NONE && m_FragmentCount == 1)
			Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
	}

	uint32_t DirectXColorBuffer::ComputeNumMips(uint32_t Width, uint32_t Height)
	{
		uint32_t HightBit;
		_BitScanReverse((unsigned long*)&HightBit, Width | Height);
		return HightBit + 1;
	}

	void DirectXColorBuffer::CreateDerivedViews(ImageFormat Format, uint32_t ArraySize, uint32_t NumMips, Ref<Device> pDevice)
	{
		PX_CORE_ASSERT(ArraySize == 1 || NumMips == 1, "we don't support auto-mips on texture arrays");

		m_NumMipMaps = NumMips - 1;

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

		RTVDesc.Format = FormatToDXGIFormat(Format);
		UAVDesc.Format = GetUAVFormat(FormatToDXGIFormat(Format));
		SRVDesc.Format = FormatToDXGIFormat(Format);
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

		Ref<DirectXDescriptorCpuHandle> rtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_RTVHandle);
		Ref<DirectXDescriptorCpuHandle> srvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRVHandle);

		if (srvHandle->GetCpuPtr() == -1)
		{
			rtvHandle->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::RTV, 1, pDevice))->GetCpuHandle());
			srvHandle->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1, pDevice))->GetCpuHandle());
		}

		ID3D12Resource* pResource = std::static_pointer_cast<DirectXGpuResource>(m_pResource)->GetResource();

		//create the render target view
		std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateRenderTargetView(pResource, &RTVDesc, rtvHandle->GetCpuHandle());

		//create the shader resource view
		std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateShaderResourceView(pResource, &SRVDesc, srvHandle->GetCpuHandle());

		if (m_FragmentCount > 1) return;

		for (uint32_t i = 0; i < NumMips; ++i)
		{
			Ref<DirectXDescriptorCpuHandle> uavHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAVHandle[i]);

			if (uavHandle->GetCpuPtr() == -1)
				uavHandle->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1, pDevice))->GetCpuHandle());

			std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &UAVDesc, uavHandle->GetCpuHandle());

			++UAVDesc.Texture2D.MipSlice;
		}
	}

}