#pragma once

#include "DirectXContext.h"

#include "DescriptorHandle.h"

namespace Pixel {

	class DescriptorAllocator
	{
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE Type);

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t Count);

		static void DestroyAll(void);

		static D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t Count = 1);
	protected:
		//per heap has descriptor number
		static const uint32_t sm_NumDescriptorPerHeap = 256;
		static std::mutex sm_AllocationMutex;
		//descriptor heap(pool)
		static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool;

		//request new descriptor heap, parameter is new type
		static ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type);

		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
		ID3D12DescriptorHeap* m_CurrentHeap;

		//cpu handle
		D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;

		uint32_t m_DescriptorSize;
		//remaining free handle
		uint32_t m_RemainingFreeHandles;
	};

	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();

		//MaxCount: the descriptor heap's descriptor number
		void Create(const std::wstring& DebugHeapName, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t MaxCount);

		void Destroy();

		bool HasAvailableSpace(uint32_t Count) const;
		DescriptorHandle Alloc(uint32_t Count = 1);

		DescriptorHandle operator[](uint32_t arrayIndex) const { return m_FirstHandle + arrayIndex * m_DescriptorSize; }

		uint32_t GetOffsetOfHandle(const DescriptorHandle& DHandle);

		//check the DHandle is in the [m_FirstHandle, m_NextFreeHandle - 1] range
		bool ValidateHandle(const DescriptorHandle& DHandle) const;

		ID3D12DescriptorHeap* GetHeapPointer() const { return m_Heap.Get(); }

		uint32_t GetDescriptorSize() const { return m_DescriptorSize; }
	private:

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		uint32_t m_DescriptorSize;

		//left free descriptors
		uint32_t m_NumFreeDescriptors;
		DescriptorHandle m_FirstHandle;
		DescriptorHandle m_NextFreeHandle;
	};
}