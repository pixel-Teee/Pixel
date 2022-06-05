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

	DescriptorHandle& DescriptorHandle::operator+=(int32_t OffsetScaledByDescriptorSize)
	{
		*m_CpuHandle += OffsetScaledByDescriptorSize;
		*m_GpuHandle += OffsetScaledByDescriptorSize;
		return *this;
	}

}