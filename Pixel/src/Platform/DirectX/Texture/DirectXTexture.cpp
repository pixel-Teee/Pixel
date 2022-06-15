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

	//---only use for english------
	std::wstring StringToWString(const std::string& str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;
	}
	//---only use for english------
	DirectXTexture::DirectXTexture(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat)
	{
		//m_pGpuResource->m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;

		//m_Width = width;
		//m_Height = height;
		//
		//D3D12_RESOURCE_DESC texDesc = {};
		//texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//texDesc.Width = width;
		//texDesc.DepthOrArraySize = 1;
		//texDesc.MipLevels = 1;
		//texDesc.Format = ImageFormatToDirectXImageFormat(textureFormat);
		//texDesc.SampleDesc.Count = 1;
		//texDesc.SampleDesc.Quality = 0;
		//texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		//texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		//D3D12_HEAP_PROPERTIES HeapProps;
		//HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		//HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		//HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		//HeapProps.CreationNodeMask = 1;
		//HeapProps.VisibleNodeMask = 1;

		//PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateCommittedResource(&HeapProps,
		//	D3D12_HEAP_FLAG_NONE, &texDesc, m_pGpuResource->m_UsageState, nullptr, IID_PPV_ARGS(m_pGpuResource->m_pResource.ReleaseAndGetAddressOf())) >= 0, "create texture 2d error!");

		//m_pGpuResource->m_pResource->SetName(L"Texture");

		////D3D12_SUBRESOURCE_DATA texResource;
		//////texResource.pData = 
		////texResource.pData = nullptr;
		////texResource.RowPitch = RowPitch;
		////texResource.SlicePitch = RowPitch * width;

		////Ref<Context> pContext = pContextManager->AllocateContext(CommandListType::Graphics, DirectXDevice::Get());
		////std::static_pointer_cast<GraphicsContext>(pContext)->InitializeTexture(*m_pGpuResource, 1, &texResource, pContextManager, DirectXDevice::Get());

		//m_pHandle = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::CBV_UAV_SRV, 1);

		//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//srvDesc.Format = ImageFormatToDirectXImageFormat(textureFormat);
		//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.TextureCube.MipLevels = 1;
		//srvDesc.TextureCube.MostDetailedMip = 0;
		//srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		//std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateShaderResourceView(m_pGpuResource->m_pResource.Get(), &srvDesc, std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_pHandle->GetCpuHandle())->GetCpuHandle());
	}

	DirectXTexture::DirectXTexture(const std::string& path):m_path(path)
	{
		m_pGpuResource = CreateRef<DirectXGpuResource>(ResourceStates::CopyDest);

		//get extension

		//size_t dotPlace = path.find_last_of('.');

		//std::string ddsFilePath = path.substr(0, dotPlace - 1) + ".dds";

		//------need to refractor------
		Ref<GpuResource> pResource = m_pGpuResource;
		PX_CORE_ASSERT(DirectX::CreateDDSTextureForPixelEngine(*pResource, StringToWString(path).c_str()) >= 0,
			"create texture 2d error!");

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

	Ref<DescriptorCpuHandle> DirectXTexture::GetCpuDescriptorHandle() const
	{
		return m_pHandle->GetCpuHandle();
	}

	void DirectXTexture::SetData(void* data, uint32_t size)
	{
		D3D12_SUBRESOURCE_DATA texResource;
		texResource.pData = data;
		texResource.RowPitch = size / m_Height;//need to refactoring
		texResource.SlicePitch = size;

		//GraphicsContext::InitializeTexture()

		Ref<Context> pContext = DirectXDevice::Get()->GetContextManager()->CreateGraphicsContext(L"Create Texture");

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