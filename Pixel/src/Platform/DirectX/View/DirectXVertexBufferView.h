#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/VertexBufferView.h"

namespace Pixel {
	class GpuVirtualAddress;
	class DirectXVBV : public VBV
	{
	public:
		DirectXVBV(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, uint32_t Stride);
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	private:
		Ref<GpuVirtualAddress> m_GpuVirtualAddress;
		D3D12_VERTEX_BUFFER_VIEW m_VBView;
		size_t OffSet;
		uint32_t Size;
		uint32_t Stride;//one vertex's stride
	};
}