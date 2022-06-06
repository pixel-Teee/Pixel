#include "pxpch.h"

#include "DescriptorHandle.h"
#include "DescriptorCpuHandle.h"
#include "DescriptorGpuHandle.h"

namespace Pixel {

	DescriptorHandle::DescriptorHandle()
	{
		m_CpuHandle = DescriptorCpuHandle::Create();
		m_GpuHandle = DescriptorGpuHandle::Create();
	}

	DescriptorHandle::DescriptorHandle(Ref<DescriptorCpuHandle> cpuHandle, Ref<DescriptorGpuHandle> gpuHandle)
	{
		m_CpuHandle = cpuHandle;
		m_GpuHandle = gpuHandle;
	}

	DescriptorHandle& DescriptorHandle::operator+=(int32_t OffsetScaledByDescriptorSize)
	{
		*m_CpuHandle += OffsetScaledByDescriptorSize;
		*m_GpuHandle += OffsetScaledByDescriptorSize;
		return *this;
	}

	DescriptorHandle DescriptorHandle::operator+(uint32_t OffsetScaledByDescriptorSize)
	{
		DescriptorHandle handle = *this;
		*handle.m_CpuHandle += OffsetScaledByDescriptorSize;
		*handle.m_GpuHandle += OffsetScaledByDescriptorSize;
		return handle;
	}

}