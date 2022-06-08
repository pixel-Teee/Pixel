#pragma once

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"

namespace Pixel {

	class DirectXDescriptorHeap : public DescriptorHeap
	{
	public:
		DirectXDescriptorHeap(const std::wstring& DebugName, DescriptorHeapType Type, uint32_t MaxCount);

		virtual bool HasAvailableSpace(uint32_t Count) const override;

		virtual Ref<DescriptorHandle> Alloc(uint32_t Count = 1) override;

		virtual DescriptorHandle operator[](uint32_t arrayIndex) const override;

		virtual uint32_t GetOffsetOfHandle(const DescriptorHandle& handle) override;

		virtual bool ValidateHandle(const DescriptorHandle& handle) const override;

		ID3D12DescriptorHeap* GetHeapPointer();

		virtual uint32_t GetDescriptorSize() const override;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		uint32_t m_DescriptorSize;
		uint32_t m_NumFreeDescriptors;
		Ref<DescriptorHandle> m_FirstHandle;
		Ref<DescriptorHandle> m_NextFreeHandle;
	};
}
