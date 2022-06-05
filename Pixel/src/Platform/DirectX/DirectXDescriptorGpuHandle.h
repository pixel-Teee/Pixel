#pragma once

#include "d3dx12.h"
#include "Pixel/Renderer/DescriptorGpuHandle.h"

namespace Pixel {
	class DirectXDescriptorGpuHandle : public DescriptorGpuHandle
	{
	public:
		DirectXDescriptorGpuHandle();

		virtual bool IsShaderVisible() const override;
		virtual uint64_t GetGpuPtr() const override;

		operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }
		virtual DescriptorGpuHandle& operator+=(int32_t OffsetScaledByDescriptorSize) override;
		DescriptorGpuHandle& operator+(int32_t OffsetScaledByDescriptorSize);
	private:
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	};
}