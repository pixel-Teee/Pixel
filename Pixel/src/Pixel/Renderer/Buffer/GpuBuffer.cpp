#include "pxpch.h"

#include "GpuBuffer.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Pixel/Renderer/Device/Device.h"

namespace Pixel {

	Ref<GpuBuffer> GpuBuffer::Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, Ref<Device> pDevice, const void* initialData /*= nullptr*/)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::DirectX12:
			{
				Ref<DirectXGpuBuffer> pGpuBuffer = std::make_shared<DirectXGpuBuffer>();
				pGpuBuffer->Create(name, NumElements, ElementSize, pDevice, initialData);
				return pGpuBuffer;
			}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}