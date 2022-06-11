#include "pxpch.h"
#include "DescriptorHeap.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorHeap.h"

namespace Pixel {

	Ref<DescriptorHeap> DescriptorHeap::Create(const std::wstring& DebugName, DescriptorHeapType Type, uint32_t MaxCount, Ref<Device> pDevice)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDescriptorHeap>(DebugName, Type, MaxCount, pDevice);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<DescriptorHeap> DescriptorHeap::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDescriptorHeap>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}