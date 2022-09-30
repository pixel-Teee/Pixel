#include "pxpch.h"
#include "DescriptorHandle.h"

namespace Pixel {
	//------Descriptor Handle------
	DescriptorHandle::DescriptorHandle()
	{
		m_CpuHandle.ptr = -1;
		m_GpuHandle.ptr = -1;
	}

	DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle)
		:m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle)
	{

	}

	DescriptorHandle DescriptorHandle::operator+(int32_t OffsetScaledByDescriptorSize) const
	{
		DescriptorHandle ret = *this;
		ret += OffsetScaledByDescriptorSize;
		return ret;
	}

	void DescriptorHandle::operator+=(int32_t OffsetScaledByDescriptorSize)
	{
		if (m_CpuHandle.ptr != -1)
			m_CpuHandle.ptr += OffsetScaledByDescriptorSize;
		if (m_GpuHandle.ptr != -1)
			m_GpuHandle.ptr += OffsetScaledByDescriptorSize;
	}
	//------Descriptor Handle------
}