#include "pxpch.h"
#include "RasterState.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/State/DirectXRasterState.h"


namespace Pixel {

	RasterState::~RasterState()
	{

	}

	Ref<RasterState> RasterState::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXRasterState>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}