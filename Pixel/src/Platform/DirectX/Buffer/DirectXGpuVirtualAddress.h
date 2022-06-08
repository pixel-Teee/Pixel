#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/GpuVirtualAddress.h"

namespace Pixel {
	class DirectXGpuVirtualAddress : public GpuVirtualAddress
	{
	public:
		DirectXGpuVirtualAddress();
		void SetGpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress);
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress();
	private:
		D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
	};
}