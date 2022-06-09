#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/IndexBufferView.h"

namespace Pixel {
	class GpuVirtualAddress;
	class DirectXIBV : public IBV
	{
	public:
		DirectXIBV();
		DirectXIBV(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, bool b32Bit);
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	private:
		Ref<GpuVirtualAddress> m_GpuVirtualAddress;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		size_t m_offset;
		uint32_t m_size;
		bool m_b32Bit;
	};
}