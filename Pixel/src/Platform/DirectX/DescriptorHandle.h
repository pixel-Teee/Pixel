#pragma once

#include "d3dx12.h"

namespace Pixel {
	//this handle refers to a descriptor or a descriptor table
	class DescriptorHandle
	{
	public:
		DescriptorHandle();

		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle);

		DescriptorHandle operator+(int32_t OffsetScaledByDescriptorSize) const;

		void operator += (int32_t OffsetScaledByDescriptorSize);

		const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &m_CpuHandle; }
		operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }
		operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }

		size_t GetCpuPtr() const { return m_CpuHandle.ptr; }
		uint64_t GetGpuPtr() const { return m_GpuHandle.ptr; }

		bool IsNull() const { return m_CpuHandle.ptr == -1; }
		bool IsShaderVisible() const { return m_GpuHandle.ptr != -1; }
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	};

}