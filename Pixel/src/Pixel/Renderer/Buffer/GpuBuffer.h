#pragma once

namespace Pixel {
	class DescriptorCpuHandle;
	class GpuBuffer
	{
	public:

		virtual Ref<DescriptorCpuHandle> GetUAV() const = 0;
		virtual Ref<DescriptorCpuHandle> GetSRV() const = 0;

		static Ref<GpuBuffer> Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData = nullptr);
	};
}