#include "pxpch.h"
#include "PipelineLibrary.h"

#include "Pixel/Renderer/Renderer.h"

#include "Platform/DirectX/PipelineStateObject/DirectXPipelineLibrary.h"

namespace Pixel {
	//create directx pipeline library
	Ref<PipelineLibrary> PipelineLibrary::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXPipelineLibrary>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}