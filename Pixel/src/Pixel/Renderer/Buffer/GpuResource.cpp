#include "pxpch.h"

#include "GpuResource.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"

namespace Pixel {

	GpuResource::~GpuResource()
	{

	}

	Ref<GpuResource> GpuResource::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXGpuResource>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<GpuResource> GpuResource::Create(ResourceStates CurrentState)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXGpuResource>(CurrentState);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}