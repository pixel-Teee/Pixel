#pragma once

#include "Pixel/Renderer/Descriptor/HeapType.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"

namespace Pixel {

	class DescriptorHeap {
	public:
		virtual bool HasAvailableSpace(uint32_t Count) const = 0;

		virtual Ref<DescriptorHandle> Alloc(uint32_t Count = 1) = 0;

		virtual DescriptorHandle operator[](uint32_t arrayIndex) const = 0;

		virtual uint32_t GetOffsetOfHandle(const DescriptorHandle& handle) = 0;

		virtual bool ValidateHandle(const DescriptorHandle& handle) const = 0;

		virtual uint32_t GetDescriptorSize() const = 0;

		static Ref<DescriptorHeap> Create(const std::wstring& DebugName, DescriptorHeapType Type, uint32_t MaxCount);
	};
}
