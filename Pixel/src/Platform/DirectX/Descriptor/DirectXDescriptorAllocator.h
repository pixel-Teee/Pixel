#pragma once

#include <vector>

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"

namespace Pixel {
	class Device;
	class DirectXDescriptorAllocator : public DescriptorAllocator
	{
	public:
		DirectXDescriptorAllocator(DescriptorHeapType Type);

		virtual Ref<DescriptorCpuHandle> Allocate(uint32_t Count) override;

		virtual Ref<DescriptorHandle> AllocateCpuAndGpuHandle(uint32_t Count) override;

		virtual void DestroyDescriptor() override;

		virtual uint32_t GetDescriptorSize() override;
	protected:
		//per heap has descriptor number
		static const uint32_t sm_NumDescriptorPerHeap = 256;
		static std::mutex sm_AllocationMutex;
		//descriptor heap(pool)
		std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool;

		//request new descriptor heap, parameter is new type
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type);

		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;

		//------important------
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CurrentHeap;
		//------important------
		
		//cpu handle
		D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;

		//handle size
		uint32_t m_DescriptorSize;

		//offset
		uint32_t m_Offset;

		//remaining free handle
		uint32_t m_RemainingFreeHandles;
	};

}