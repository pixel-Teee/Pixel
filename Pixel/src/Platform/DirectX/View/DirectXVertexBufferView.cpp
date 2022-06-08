#include "pxpch.h"
#include "DirectXVertexBufferView.h"
#include "Platform/DirectX/Buffer/DirectXGpuVirtualAddress.h"

namespace Pixel {

	DirectXVBV::DirectXVBV(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, uint32_t Stride)
	{
		m_GpuVirtualAddress = pGpuVirtualAddress;
		m_VBView.BufferLocation = std::static_pointer_cast<DirectXGpuVirtualAddress>(m_GpuVirtualAddress)->GetGpuVirtualAddress() + OffSet;
		m_VBView.SizeInBytes = Size;
		m_VBView.StrideInBytes = Stride;
	}

}