#include "pxpch.h"

#include "Pixel/Renderer/Renderer.h"
#include "GpuVirtualAddress.h"
#include "Platform/DirectX/Buffer/DirectXGpuVirtualAddress.h"

namespace Pixel {

	Ref<GpuVirtualAddress> GpuVirtualAddress::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXGpuVirtualAddress>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}