#pragma once



namespace Pixel {
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;

	class DescriptorHandle
	{
	public:
		DescriptorHandle();

		DescriptorHandle& operator+=(int32_t OffsetScaledByDescriptorSize);

		size_t GetCpuPtr() const { return m_CpuHandle->GetCpuPtr(); }
		uint64_t GetGpuPtr() const { return m_GpuHandle->GetGpuPtr(); }
		bool IsNull() const { return m_CpuHandle->IsNull(); }
		bool IsShaderVisible() const { return m_GpuHandle->IsShaderVisible(); }

	private:
		Ref<DescriptorCpuHandle> m_CpuHandle;
		Ref<DescriptorGpuHandle> m_GpuHandle;
	};
}