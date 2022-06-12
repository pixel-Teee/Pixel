#include "pxpch.h"

#include "DepthState.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/State/DirectXDepthState.h"

namespace Pixel {

	DepthState::~DepthState()
	{

	}

	Ref<DepthState> DepthState::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDepthState>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}