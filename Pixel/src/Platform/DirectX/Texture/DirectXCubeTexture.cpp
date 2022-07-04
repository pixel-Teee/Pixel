#include "pxpch.h"

#include "DirectXCubeTexture.h"

#include "Platform/DirectX/Buffer/DirectXGpuResource.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	DirectXCubeTexture::DirectXCubeTexture(uint32_t width, uint32_t height, ImageFormat format)
	{
		//create resource
		if (m_pCubeTextureResource != nullptr) m_pCubeTextureResource->Destroy();

		m_pCubeTextureResource = CreateRef<DirectXGpuResource>(ResourceStates::GenericRead);
		//create rtv handle and srv handle

		m_Width = width;
		m_Height = height;

		D3D12_RESOURCE_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.DepthOrArraySize = 6;
		texDesc.MipLevels = 1;
		texDesc.Format = ImageFormatToDirectXImageFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		Ref<DirectXGpuResource> pResource = std::static_pointer_cast<DirectXGpuResource>(m_pCubeTextureResource);

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateCommittedResource
		(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc, pResource->m_UsageState, nullptr, IID_PPV_ARGS(pResource->m_pResource.ReleaseAndGetAddressOf())) >= 0,
		"create cube texture resource error!");

		m_SrvHandles = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::CBV_UAV_SRV, 1);
		Ref<DirectXDescriptorCpuHandle> pHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SrvHandles->GetCpuHandle());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = ImageFormatToDirectXImageFormat(format);
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateShaderResourceView(pResource->m_pResource.Get(), &srvDesc,
			pHandle->GetCpuHandle());

		//create rtv
		for (uint32_t i = 0; i < 6; ++i)
		{
			m_RtvHandles[i] = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::RTV, 1);
			Ref<DirectXDescriptorCpuHandle> pHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_RtvHandles[i]->GetCpuHandle());

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;//2D Array
			rtvDesc.Format = ImageFormatToDirectXImageFormat(format);
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.PlaneSlice = 0;
			rtvDesc.Texture2DArray.FirstArraySlice = i;

			rtvDesc.Texture2DArray.ArraySize = 1;

			std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateRenderTargetView(pResource->m_pResource.Get(), &rtvDesc,
				pHandle->GetCpuHandle());
		}
	}

	DirectXCubeTexture::DirectXCubeTexture(uint32_t width, uint32_t height, ImageFormat format, uint32_t MaxMipLevels)
	{
		if (m_pCubeTextureResource != nullptr) m_pCubeTextureResource->Destroy();

		m_pCubeTextureResource = CreateRef<DirectXGpuResource>(ResourceStates::GenericRead);

		m_Width = width;
		m_Height = height;

		D3D12_RESOURCE_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.DepthOrArraySize = 6;
		texDesc.MipLevels = MaxMipLevels;//important
		texDesc.Format = ImageFormatToDirectXImageFormat(format);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}

	DirectXCubeTexture::~DirectXCubeTexture()
	{

	}

	Ref<DescriptorHandle> DirectXCubeTexture::GetRtvHandle(uint32_t index)
	{
		return m_RtvHandles[index];
	}

	Ref<DescriptorHandle> DirectXCubeTexture::GetSrvHandle()
	{
		return m_SrvHandles;
	}

}