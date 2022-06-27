#pragma once

#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"

namespace Pixel {

	class DescriptorCpuHandle;
	class Device;
	class DescriptorAllocator
	{
	public:
		virtual Ref<DescriptorCpuHandle> Allocate(uint32_t Count) = 0;

		virtual Ref<DescriptorHandle> AllocateCpuAndGpuHandle(uint32_t Count) = 0;

		virtual void DestroyDescriptor() = 0;

		static Ref<DescriptorAllocator> Create(DescriptorHeapType HeapType);

		//use this function to allocate descriptor
		static Ref<DescriptorCpuHandle> AllocateDescriptor(DescriptorHeapType Type, uint32_t Count);

		static Ref<DescriptorHandle> AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType Type, uint32_t Count);

		virtual uint32_t GetDescriptorSize() = 0;
		//static void DestroyDescriptorPools();

		//static Ref<DescriptorAllocator> g_Descriptor[(uint64_t)DescriptorHeapType::DescriptorHeapTypeNums];
	};
}