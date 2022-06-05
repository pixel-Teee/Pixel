#include "pxpch.h"

#include "DescriptorCpuHandle.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	DescriptorCpuHandle::~DescriptorCpuHandle()
	{

	}

	Ref<DescriptorCpuHandle> DescriptorCpuHandle::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDescriptorCpuHandle>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}