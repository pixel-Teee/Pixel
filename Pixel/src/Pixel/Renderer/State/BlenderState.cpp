#include "pxpch.h"

#include "BlenderState.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/State/DirectXBlenderState.h"

namespace Pixel {

	BlenderState::~BlenderState()
	{

	}

	Ref<BlenderState> BlenderState::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXBlenderState>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}