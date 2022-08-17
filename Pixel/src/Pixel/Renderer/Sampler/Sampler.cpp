#include "pxpch.h"

//-----my library------
#include "Sampler.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"
#include "Pixel/Renderer/Renderer.h"
//-----my library------

namespace Pixel {

	Ref<SamplerDesc> SamplerDesc::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXSamplerDesc>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}