#include "pxpch.h"

#include "PipelineStateObject.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"

namespace Pixel {

	Ref<PSO> PSO::CreateGraphicsPso(const wchar_t* Name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<GraphicsPSO>(Name);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}