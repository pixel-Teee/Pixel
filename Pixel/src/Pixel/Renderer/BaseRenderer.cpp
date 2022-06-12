#include "pxpch.h"

#include "BaseRenderer.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXRenderer.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	BaseRenderer::~BaseRenderer()
	{

	}

	Ref<BaseRenderer> BaseRenderer::Create(Ref<Device> pDevice)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return CreateRef<DirectXRenderer>(pDevice);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}