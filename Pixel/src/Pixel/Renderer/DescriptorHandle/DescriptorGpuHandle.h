#pragma once

namespace Pixel {
	class DescriptorGpuHandle {
	public:
		virtual ~DescriptorGpuHandle();

		//------interface------
		virtual uint64_t GetGpuPtr() const = 0;
		virtual bool IsShaderVisible() const = 0;
		virtual DescriptorGpuHandle& operator+=(int32_t OffsetScaledByDescriptorSize) = 0;
		//------interface------

		static Ref<DescriptorGpuHandle> Create();
	};
}