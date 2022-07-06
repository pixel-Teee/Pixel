#pragma once

#include <memory>

#include "Pixel/Renderer/Descriptor/HeapType.h"

namespace Pixel {
	class ContextManager;
	class DescriptorAllocator;
	class DescriptorCpuHandle;
	class GpuResource;
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		virtual ~Device();

		virtual Ref<ContextManager> GetContextManager() = 0;

		virtual void CopyDescriptorsSimple(uint32_t NumDescriptors, Ref<DescriptorCpuHandle> DestHandle, Ref<DescriptorCpuHandle> SrcHandle, DescriptorHeapType Type) = 0;

		virtual Ref<DescriptorAllocator> GetDescriptorAllocator(uint32_t index) = 0;

		virtual void SetClientSize(uint32_t width, uint32_t height) = 0;

		virtual Ref<GpuResource> GetCurrentBackBuffer() = 0;

		virtual void ReCreateSwapChain() = 0;

		static Ref<Device> Get();

		//test
		static void SetNull();

		static Ref<Device> m_pDevice;
	};
}
