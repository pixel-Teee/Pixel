#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"

namespace Pixel {
	class DirectXDescriptorGpuHandle : public DescriptorGpuHandle
	{
	public:
		DirectXDescriptorGpuHandle();

		virtual bool IsShaderVisible() const override;
		virtual uint64_t GetGpuPtr() const override;

		void SetGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) { m_GpuHandle = gpuHandle; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() { return m_GpuHandle; }

		operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }
		virtual DescriptorGpuHandle& operator+=(int32_t OffsetScaledByDescriptorSize) override;
		DirectXDescriptorGpuHandle operator+(int32_t OffsetScaledByDescriptorSize);
	private:
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	};
}