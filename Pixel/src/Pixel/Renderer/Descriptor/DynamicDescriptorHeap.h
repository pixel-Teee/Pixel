#pragma once

#include "HeapType.h"

namespace Pixel {
	class Context;
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;
	class RootSignature;
	class Device;
	class DynamicDescriptorHeap {
	public:
		virtual void CleanupUsedHeaps(uint64_t fenceValue) = 0;

		virtual void SetGraphicsDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[]) = 0;

		virtual void SetComputeDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[]) = 0;

		virtual Ref<DescriptorGpuHandle> UploadDirect(Ref<DescriptorCpuHandle> Handles) = 0;

		virtual void ParseGraphicsRootSignature(const RootSignature& RootSig) = 0;

		virtual void ParseComputeRootSignature(const RootSignature& RootSig) = 0;

		static Ref<DynamicDescriptorHeap> Create(Context& OwingContext, DescriptorHeapType HeapType);
	};
}