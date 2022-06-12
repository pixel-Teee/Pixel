#include "pxpch.h"

#include "Sampler.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"

namespace Pixel {

	Ref<SamplerDesc> SamplerDesc::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<SamplerDesc>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}