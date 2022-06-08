#include "pxpch.h"

#include "RootSignature.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"

namespace Pixel {

	Ref<RootSignature> RootSignature::Create(uint32_t NumRootParams, uint32_t NumStaticSampler)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXRootSignature>(NumRootParams, NumStaticSampler);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}