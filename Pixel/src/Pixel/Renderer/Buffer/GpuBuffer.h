#pragma once

namespace Pixel {
	class DescriptorCpuHandle;
	class Device;
	class GpuResource;
	class GpuBuffer
	{
	public:

		virtual void SetGpuResource(Ref<GpuResource> pGpuResource) = 0;
		virtual Ref<DescriptorCpuHandle> GetUAV() const = 0;
		virtual Ref<DescriptorCpuHandle> GetSRV() const = 0;

		static Ref<GpuBuffer> Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, Ref<Device> pDevice, const void* initialData = nullptr);
	};
}