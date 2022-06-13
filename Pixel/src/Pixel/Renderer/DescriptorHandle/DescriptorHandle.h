#pragma once

namespace Pixel {
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;

	class DescriptorHandle
	{
	public:
		DescriptorHandle();
		DescriptorHandle(Ref<DescriptorCpuHandle> cpuHandle, Ref<DescriptorGpuHandle> gpuHandle);
		DescriptorHandle(const DescriptorHandle& rhs);//deep copy

		DescriptorHandle& operator+=(int32_t OffsetScaledByDescriptorSize);
		DescriptorHandle operator+(uint32_t OffsetScaledByDescriptorSize);

		size_t GetCpuPtr() const;
		uint64_t GetGpuPtr() const;
		bool IsNull() const;
		bool IsShaderVisible() const;

		Ref<DescriptorCpuHandle> GetCpuHandle() { return m_CpuHandle; }
		Ref<DescriptorGpuHandle> GetGpuHandle() { return m_GpuHandle; }
	private:
		Ref<DescriptorCpuHandle> m_CpuHandle;
		Ref<DescriptorGpuHandle> m_GpuHandle;
	};
}