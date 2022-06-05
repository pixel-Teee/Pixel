#include "pxpch.h"
#include "IndexBufferView.h"

#include "Renderer.h"
#include "Platform/DirectX/DirectXIndexBufferView.h"
#include "Pixel/Renderer/GpuVirtualAddress.h"

namespace Pixel {

	Ref<IBV> IBV::Create(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, bool b32Bit)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12:
		{
			return std::make_shared<IBV>(pGpuVirtualAddress, OffSet, Size, b32Bit);
		}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}