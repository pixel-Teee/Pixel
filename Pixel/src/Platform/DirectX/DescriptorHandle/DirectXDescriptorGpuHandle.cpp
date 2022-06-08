#include "pxpch.h"

#include "DirectXDescriptorGpuHandle.h"

namespace Pixel {

	DirectXDescriptorGpuHandle::DirectXDescriptorGpuHandle()
	{
		m_GpuHandle.ptr = -1;
	}

	bool DirectXDescriptorGpuHandle::IsShaderVisible() const
	{
		return m_GpuHandle.ptr != -1;
	}

	uint64_t DirectXDescriptorGpuHandle::GetGpuPtr() const
	{
		return m_GpuHandle.ptr;
	}

	DescriptorGpuHandle& DirectXDescriptorGpuHandle::operator+=(int32_t OffsetScaledByDescriptorSize)
	{
		if (m_GpuHandle.ptr != -1)
			m_GpuHandle.ptr += OffsetScaledByDescriptorSize;
		return *this;
	}

	DirectXDescriptorGpuHandle DirectXDescriptorGpuHandle::operator+(int32_t OffsetScaledByDescriptorSize)
	{
		DirectXDescriptorGpuHandle Ret = *this;
		Ret += OffsetScaledByDescriptorSize;
		return Ret;
	}

}