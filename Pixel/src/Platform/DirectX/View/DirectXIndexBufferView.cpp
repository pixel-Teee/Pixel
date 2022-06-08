#include "pxpch.h"

#include "Platform/DirectX/d3dx12.h"
#include "Platform/DirectX/View/DirectXIndexBufferView.h"
#include "Platform/DirectX/Buffer/DirectXGpuVirtualAddress.h"

namespace Pixel {

	DirectXIBV::DirectXIBV(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, bool b32Bit)
	{
		m_GpuVirtualAddress = pGpuVirtualAddress;
		m_offset = OffSet;
		m_size = Size;
		m_b32Bit = b32Bit;
		
		m_IndexBufferView.BufferLocation = std::static_pointer_cast<DirectXGpuVirtualAddress>(m_GpuVirtualAddress)->GetGpuVirtualAddress() + OffSet;
		m_IndexBufferView.Format = m_b32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		m_IndexBufferView.SizeInBytes = m_size;
	}

	DirectXIBV::DirectXIBV()
	{

	}

}