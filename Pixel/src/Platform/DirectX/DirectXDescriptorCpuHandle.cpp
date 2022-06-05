#include "pxpch.h"

#include "DirectXDescriptorCpuHandle.h"

namespace Pixel {

	DirectXDescriptorCpuHandle::DirectXDescriptorCpuHandle()
	{
		m_CpuHandle.ptr = -1;
	}

	size_t DirectXDescriptorCpuHandle::GetCpuPtr() const
	{
		return m_CpuHandle.ptr;
	}

	bool DirectXDescriptorCpuHandle::IsNull() const
	{
		return m_CpuHandle.ptr == -1;
	}

	DescriptorCpuHandle& DirectXDescriptorCpuHandle::operator+=(int32_t OffsetScaledByDescriptorSize)
	{
		if (m_CpuHandle.ptr != -1)
			m_CpuHandle.ptr += OffsetScaledByDescriptorSize;
	}

	DescriptorCpuHandle& DirectXDescriptorCpuHandle::operator+(int32_t OffsetScaledByDescriptorSize)
	{
		DirectXDescriptorCpuHandle Ret = *this;
		Ret += OffsetScaledByDescriptorSize;
		return *this;
	}

}