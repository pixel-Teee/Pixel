#include "pxpch.h"

#include "DirectXGpuVirtualAddress.h"

namespace Pixel {

	DirectXGpuVirtualAddress::DirectXGpuVirtualAddress()
	{

	}

	void DirectXGpuVirtualAddress::SetGpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress)
	{
		m_GpuVirtualAddress = GpuVirtualAddress;
	}

	D3D12_GPU_VIRTUAL_ADDRESS DirectXGpuVirtualAddress::GetGpuVirtualAddress()
	{
		return m_GpuVirtualAddress;
	}

}