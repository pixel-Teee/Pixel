#include "pxpch.h"

#include "DirectXTexture.h"

#include "stb_image.h"

#include "DDSTextureLoader.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"


namespace Pixel {

	//---only use for ascill------
	std::wstring StringToWString(const std::string& str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;
	}
	//---only use for ascill------
	DirectXTexture::DirectXTexture(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat,
	const void* InitialData)
	{
		//m_pGpuResource->Destroy();
		if (m_pGpuResource != nullptr) m_pGpuResource->Destroy();

		m_pGpuResource = CreateRef<DirectXGpuResource>(ResourceStates::CopyDest);
		//m_pGpuResource->SetInitializeResourceState(ResourceStates::CopyDest);

		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;
		
		//m_Depth = 1;

		D3D12_RESOURCE_DESC texDesc = {};
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = ImageFormatToDirectXImageFormat(textureFormat);
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

		Ref<DirectXGpuResource> pResource = std::static_pointer_cast<DirectXGpuResource>(m_pGpuResource);

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateCommittedResource
		(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc, pResource->m_UsageState,
		nullptr, IID_PPV_ARGS(pResource->m_pResource.ReleaseAndGetAddressOf())) >= 0,
		"create texture resource error!");
		
		pResource->m_pResource->SetName(L"Texture");

		if (InitialData != nullptr)
		{
			D3D12_SUBRESOURCE_DATA texResource;
			texResource.pData = InitialData;
			texResource.RowPitch = RowPitch;//one row's byte size
			texResource.SlicePitch = RowPitch * height;

			Ref<Context> pContext = Device::Get()->GetContextManager()->CreateGraphicsContext(L"Create Texture");
			Ref<GraphicsContext> pGraphicsContext = std::static_pointer_cast<GraphicsContext>(pContext);
			pGraphicsContext->InitializeTexture(*pResource, 1, &texResource);

		}	

		m_pHandle = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::CBV_UAV_SRV, 1);
		Ref<DirectXDescriptorCpuHandle> pHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_pHandle->GetCpuHandle());

		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateShaderResourceView(pResource->m_pResource.Get(), nullptr,
		pHandle->GetCpuHandle());

		//------create rtv handle------
		m_RtvHandle = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::RTV, 1);
		Ref<DirectXDescriptorCpuHandle> rtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_RtvHandle->GetCpuHandle());

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = ImageFormatToDirectXImageFormat(textureFormat);
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateRenderTargetView(pResource->m_pResource.Get(), &rtvDesc,
			rtvHandle->GetCpuHandle());
		//------create rtv handle------
	}

	DirectXTexture::DirectXTexture(const std::string& path):m_path(path)
	{
		m_pGpuResource = CreateRef<DirectXGpuResource>(ResourceStates::CopyDest);

		//get extension

		//size_t dotPlace = path.find_last_of('.');

		//std::string ddsFilePath = path.substr(0, dotPlace - 1) + ".dds";

		int width, height, channels = 0;

		stbi_info(path.c_str(), &width, &height, &channels);

		if (channels == 1)
		{
			stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 1);
			m_Width = width;
			m_Height = height;

			//------Create ID3D12Resource------
			D3D12_RESOURCE_DESC texDesc = {};
			texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			texDesc.Width = m_Width;
			texDesc.Height = m_Height;
			texDesc.DepthOrArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.Format = ImageFormatToDirectXImageFormat(ImageFormat::PX_FORMAT_R8_UNORM);
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

			Ref<DirectXGpuResource> pResource = std::static_pointer_cast<DirectXGpuResource>(m_pGpuResource);

			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateCommittedResource
			(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc, pResource->m_UsageState,
				nullptr, IID_PPV_ARGS(pResource->m_pResource.ReleaseAndGetAddressOf())) >= 0,
				"create texture resource error!");

			pResource->m_pResource->SetName(L"Texture");
			//------Create ID3D12Resource------

			Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);

			D3D12_SUBRESOURCE_DATA subData;
			subData.pData = data;
			subData.RowPitch = width;
			subData.SlicePitch = width * height;

			std::static_pointer_cast<GraphicsContext>(pContext)->InitializeTexture(*pResource, 1, &subData);

			stbi_image_free(data);
		}
		else if (channels == 4 || channels == 3)
		{
			stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
			m_Width = width;
			m_Height = height;

			//------Create ID3D12Resource------
			D3D12_RESOURCE_DESC texDesc = {};
			texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			texDesc.Width = m_Width;
			texDesc.Height = m_Height;
			texDesc.DepthOrArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.Format = ImageFormatToDirectXImageFormat(ImageFormat::PX_FORMAT_R8G8B8A8_UNORM);
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

			Ref<DirectXGpuResource> pResource = std::static_pointer_cast<DirectXGpuResource>(m_pGpuResource);

			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateCommittedResource
			(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc, pResource->m_UsageState,
				nullptr, IID_PPV_ARGS(pResource->m_pResource.ReleaseAndGetAddressOf())) >= 0,
				"create texture resource error!");

			pResource->m_pResource->SetName(L"Texture");
			//------Create ID3D12Resource------

			Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);

			D3D12_SUBRESOURCE_DATA subData;
			subData.pData = data;
			subData.RowPitch = 4 * width;
			subData.SlicePitch = 4 * width * height;

			std::static_pointer_cast<GraphicsContext>(pContext)->InitializeTexture(*pResource, 1, &subData);

			stbi_image_free(data);
		}

		//------need to refractor------
		
		//PX_CORE_ASSERT(DirectX::CreateDDSTextureForPixelEngine(*pResource, StringToWString(path).c_str()) >= 0,
		//	"create texture 2d error!");

		//------need to refractor------
		//create handle
		m_pHandle = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::CBV_UAV_SRV, 1);
		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateShaderResourceView(m_pGpuResource->m_pResource.Get(), nullptr, std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_pHandle->GetCpuHandle())->GetCpuHandle());
	}

	uint32_t DirectXTexture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t DirectXTexture::GetHeight() const
	{
		return m_Height;
	}

	uint64_t DirectXTexture::GetRendererID() const 
	{
		//return static_cast<uint32_t>(std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_pCpuDescriptorHandle)->GetCpuHandle().ptr);
		return static_cast<uint64_t>(m_pHandle->GetGpuPtr());
	}

	Ref<DescriptorCpuHandle> DirectXTexture::GetCpuDescriptorHandle()
	{
		return m_pHandle->GetCpuHandle();
	}

	Ref<DescriptorHandle> DirectXTexture::GetHandle() const
	{
		return m_pHandle;
	}

	Ref<DescriptorHandle> DirectXTexture::GetRtvHandle() const
	{
		return m_RtvHandle;
	}

	void DirectXTexture::SetData(void* data, uint32_t size)
	{
		D3D12_SUBRESOURCE_DATA texResource;
		texResource.pData = data;
		texResource.RowPitch = size / m_Height;//need to refactoring
		texResource.SlicePitch = size;

		//GraphicsContext::InitializeTexture()
		Ref<Context> pContext = DirectXDevice::Get()->GetContextManager()->CreateGraphicsContext(L"Create Texture");

		pContext->TransitionResource(*m_pGpuResource, ResourceStates::CopyDest);

		std::static_pointer_cast<GraphicsContext>(pContext)->InitializeTexture(*m_pGpuResource, 1, &texResource);
	}

	void DirectXTexture::Bind(uint32_t slot /*= 0*/) const
	{
		
	}

	std::string& DirectXTexture::GetPath()
{
		return m_path;
	}

}