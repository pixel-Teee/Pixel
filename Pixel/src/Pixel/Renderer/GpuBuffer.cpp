#include "pxpch.h"

#include "Pixel/Renderer/GpuBuffer.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXGpuBuffer.h"

namespace Pixel {

	Ref<GpuBuffer> GpuBuffer::Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData /*= nullptr*/)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::DirectX12:
			{
				Ref<DirectXGpuBuffer> pGpuBuffer = std::make_shared<DirectXGpuBuffer>();
				pGpuBuffer->Create(name, NumElements, ElementSize, initialData);
				return pGpuBuffer;
			}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}