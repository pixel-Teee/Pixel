#pragma once

#include <queue>

#include <wrl/client.h>

#include "Pixel/Renderer/Descriptor/DynamicDescriptorHeap.h"
#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"

namespace Pixel {
	class DirectXContext;
	class RootSignature;
	class Device;
	//describes a descriptor table entry : a region of handle cache and which handles have been set
	struct DescriptorTableCache
	{
		DescriptorTableCache() :AssignedHandlesBitMap(0) {};

		uint32_t AssignedHandlesBitMap;
		D3D12_CPU_DESCRIPTOR_HANDLE* TableStart;
		uint32_t TableSize;
	};

	struct DescriptorHandleCache
	{
		DescriptorHandleCache();

		void ClearCache();

		uint32_t m_RootDescriptorTablesBitMap;
		uint32_t m_StaleRootParamsBitMap;
		uint32_t m_MaxCachedDescriptors;

		static const uint32_t kMaxNumDescriptors = 256;
		static const uint32_t kMaxNumDescriptorTables = 16;

		uint32_t ComputeStagedSize();
		void CopyAndBindStaleTables(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t DescriptorSize,
			DescriptorHandle DestHandleStart, ID3D12GraphicsCommandList* CmdList,
			void(STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

		DescriptorTableCache m_RootDescriptorTable[kMaxNumDescriptorTables];
		D3D12_CPU_DESCRIPTOR_HANDLE m_HandleCache[kMaxNumDescriptors];

		void UnBindAllValid();
		//copy Handles to m_RootDescriptorTable[RootIndex]
		void StageDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[]);
		void ParseRootSignature(D3D12_DESCRIPTOR_HEAP_TYPE Type, const RootSignature& RootSig);
	};

	//this class is a linear allocation system for dynamically generated descriptor tables
	//it internally caches cpu descriptor handles so that when not enough space is available in the current heap
	//necessary descriptors can be re-copied to the new heap
	class DirectXDynamicDescriptorHeap : public DynamicDescriptorHeap
	{
	public:
		DirectXDynamicDescriptorHeap(Context& OwningContext, DescriptorHeapType HeapType);

		~DirectXDynamicDescriptorHeap();

		static void DestroyAll();

		void CleanupUsedHeaps(uint64_t fenceValue);

		//copy multiple handles into the cache are reserved for the specified root parameter
		virtual void SetGraphicsDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[]) override;

		virtual void SetComputeDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[]) override;

		//bypass the cache and upload directly to the shader-visible heap
		Ref<DescriptorGpuHandle> UploadDirect(Ref<DescriptorCpuHandle> Handles);

		virtual void ParseGraphicsRootSignature(const RootSignature& RootSig) override;

		virtual void ParseComputeRootSignature(const RootSignature& RootSig) override;

		//upload any new descriptors in the cache to the shader-visible heap
		void CommitGraphicsRootDescriptorTables(ID3D12GraphicsCommandList* CmdList);

		void CommitComputeRootDescriptorTables(ID3D12GraphicsCommandList* CmdList);
	private:

		//static numbers
		static const uint32_t kNumDescriptorPerHeap = 1024;
		static std::mutex sm_Mutex;
		static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool[2];
		static std::queue<std::pair<uint64_t, ID3D12DescriptorHeap*>> sm_RetiredDescriptorHeaps[2];
		static std::queue<ID3D12DescriptorHeap*> sm_AvailableDescriptorHeaps[2];

		//static methods
		static ID3D12DescriptorHeap* RequestDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);
		static void DiscardDescriptorHeaps(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint64_t FenceValueForReset, const std::vector<ID3D12DescriptorHeap*>& UsedHeaps);

		//non-static members
		DirectXContext& m_OwingContext;
		ID3D12DescriptorHeap* m_CurrentHeapPtr;
		const D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorType;
		uint32_t m_DescriptorSize;
		uint32_t m_CurrentOffset;
		DescriptorHandle m_FirstDescriptor;
		std::vector<ID3D12DescriptorHeap*> m_RetriedHeaps;

		DescriptorHandleCache m_GraphicsHandleCache;
		DescriptorHandleCache m_ComputeHandleCache;

		bool HasSpace(uint32_t Count);

		void RetireCurrentHeap();
		void RetireUsedHeaps(uint64_t fanceValue);
		ID3D12DescriptorHeap* GetHeapPointer();

		DescriptorHandle Allocate(uint32_t Count);

		void CopyAndBindStagedTables(DescriptorHandleCache& HandleCache, ID3D12GraphicsCommandList* CmdList,
			void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

		//mark all descriptors in the cache as stale and in need of re-uploading
		void UnBindAllValid();
	};
}
