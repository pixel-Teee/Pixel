#include "pxpch.h"

#include "RootParameter.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootParameter.h"

namespace Pixel {

	Ref<RootParameter> RootParameter::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXRootParameter>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}