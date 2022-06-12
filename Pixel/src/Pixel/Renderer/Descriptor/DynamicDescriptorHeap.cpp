#include "pxpch.h"

#include "DynamicDescriptorHeap.h"
#include "Pixel/Renderer/Renderer.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Platform/DirectX/Descriptor/DirectXDynamicDescriptorHeap.h"

namespace Pixel {

	Ref<DynamicDescriptorHeap> DynamicDescriptorHeap::Create(Context& OwingContext, DescriptorHeapType HeapType)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDynamicDescriptorHeap>(OwingContext, HeapType);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}