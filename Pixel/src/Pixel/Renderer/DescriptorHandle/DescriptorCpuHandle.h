#pragma once

namespace Pixel {
	class DescriptorCpuHandle {
	public:
		virtual ~DescriptorCpuHandle();

		//------interface------
		virtual size_t GetCpuPtr() const = 0;
		virtual bool IsNull() const = 0;
		virtual DescriptorCpuHandle& operator+=(int32_t OffsetScaledByDescriptorSize) = 0;

		virtual void SetCpuHandle(void* CpuHandle) = 0;
		//------interface------

		static Ref<DescriptorCpuHandle> Create();
	};
}