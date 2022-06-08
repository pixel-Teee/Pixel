#include "pxpch.h"

#include "DescriptorGpuHandle.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"

namespace Pixel {

	DescriptorGpuHandle::~DescriptorGpuHandle()
	{

	}

	Ref<DescriptorGpuHandle> DescriptorGpuHandle::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDescriptorGpuHandle>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}