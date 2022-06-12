#include "pxpch.h"

#include "DirectXTexture.h"

#include "Platform/DirectX/Buffer/DirectXGpuResource.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"

namespace Pixel {

	DirectXTexture::DirectXTexture(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat, Ref<ContextManager> pContextManager, Ref<Device> pDevice)
	{
		m_pGpuResource->m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;

		m_Width = width;
		m_Height = height;
		
		D3D12_RESOURCE_DESC texDesc = {};
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Width = width;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = ImageFormatToDirectXImageFormat(textureFormat);
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateCommittedResource(&HeapProps,
			D3D12_HEAP_FLAG_NONE, &texDesc, m_pGpuResource->m_UsageState, nullptr, IID_PPV_ARGS(m_pGpuResource->m_pResource.ReleaseAndGetAddressOf())) >= 0, "create texture 2d error!");

		m_pGpuResource->m_pResource->SetName(L"Texture");

		//D3D12_SUBRESOURCE_DATA texResource;
		////texResource.pData = 
		//texResource.pData = nullptr;
		//texResource.RowPitch = RowPitch;
		//texResource.SlicePitch = RowPitch * width;

		//Ref<Context> pContext = pContextManager->AllocateContext(CommandListType::Graphics, pDevice);
		//std::static_pointer_cast<GraphicsContext>(pContext)->InitializeTexture(*m_pGpuResource, 1, &texResource, pContextManager, pDevice);

		m_pCpuDescriptorHandle = DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1, pDevice);

		D3D12_CPU_DESCRIPTOR_HANDLE destHandle;
		std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateShaderResourceView(m_pGpuResource->m_pResource.Get(), nullptr, destHandle);
		m_pCpuDescriptorHandle->SetCpuHandle((void*)&destHandle);
	}

	DirectXTexture::DirectXTexture(const std::string& path)
	{
		//from this path to load texture

	}

	uint32_t DirectXTexture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t DirectXTexture::GetHeight() const
	{
		return m_Height;
	}

	uint32_t DirectXTexture::GetRendererID() const
	{
		return static_cast<uint32_t>(std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_pCpuDescriptorHandle)->GetCpuHandle().ptr);
	}

	void DirectXTexture::SetData(void* data, uint32_t size)
	{
		
	}

	void DirectXTexture::Bind(uint32_t slot /*= 0*/) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	std::string& DirectXTexture::GetPath()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}