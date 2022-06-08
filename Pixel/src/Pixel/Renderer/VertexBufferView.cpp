#include "pxpch.h"

#include "Renderer.h"
#include "VertexBufferView.h"
#include "Pixel/Renderer/GpuVirtualAddress.h"
#include "Platform/DirectX/View/DirectXVertexBufferView.h"

namespace Pixel {

	Ref<VBV> VBV::Create(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, uint32_t Stride)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12:
		{
			return std::make_shared<DirectXVBV>(pGpuVirtualAddress, OffSet, Size, Stride);
		}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}