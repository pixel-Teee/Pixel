#pragma once

#include <memory>

#include "Pixel/Renderer/Descriptor/HeapType.h"

namespace Pixel {
	class ContextManager;
	class DescriptorAllocator;
	class DescriptorCpuHandle;
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		virtual ~Device();

		virtual Ref<ContextManager> GetContextManager() = 0;

		virtual void CopyDescriptorsSimple(uint32_t NumDescriptors, Ref<DescriptorCpuHandle> DestHandle, Ref<DescriptorCpuHandle> SrcHandle, DescriptorHeapType Type) = 0;

		virtual Ref<DescriptorAllocator> GetDescriptorAllocator(uint32_t index) = 0;

		static Ref<Device> Get();

		//test
		static void SetNull();

		static Ref<Device> m_pDevice;
	};
}
