#pragma once

#include "DescriptorCpuHandle.h"
#include "DescriptorGpuHandle.h"

namespace Pixel {
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;

	class DescriptorHandle
	{
	public:
		DescriptorHandle();
		DescriptorHandle(Ref<DescriptorCpuHandle> cpuHandle, Ref<DescriptorGpuHandle> gpuHandle);

		DescriptorHandle& operator+=(int32_t OffsetScaledByDescriptorSize);
		DescriptorHandle operator+(uint32_t OffsetScaledByDescriptorSize);

		size_t GetCpuPtr() const { return m_CpuHandle->GetCpuPtr(); }
		uint64_t GetGpuPtr() const { return m_GpuHandle->GetGpuPtr(); }
		bool IsNull() const { return m_CpuHandle->IsNull(); }
		bool IsShaderVisible() const { return m_GpuHandle->IsShaderVisible(); }

		Ref<DescriptorCpuHandle> GetCpuHandle() { return m_CpuHandle; }
		Ref<DescriptorGpuHandle> GetGpuHandle() { return m_GpuHandle; }
	private:
		Ref<DescriptorCpuHandle> m_CpuHandle;
		Ref<DescriptorGpuHandle> m_GpuHandle;
	};
}