#pragma once

#include "Pixel/Renderer/Context.h"

#include <queue>
#include <wrl/client.h>
#include <dxgi1_4.h>

#include "d3dx12.h"
#include "LinearAllocator.h"
#include "DynamicDescriptorHeap.h"

struct GLFWwindow;

namespace Pixel {
	class GpuResource;
	class CommandList;
	class CommandListManager;
	class DirectXSwapChain;
	class DescriptorAllocator;
	class DirectXContext;
	class GpuBuffer;
	class PSO;
	class GraphicsContext;

	class ContextManager
	{
	public:
		ContextManager();

		//if queue is empty, then create a new context, push to vector and queue, else from the queue, extract one context
		Ref<DirectXContext> AllocateContext(D3D12_COMMAND_LIST_TYPE Type);
		//push a context into the queue
		void FreeContext(Ref<DirectXContext> UsedContext);

		//clear the sm_ContextPool(ownership is DirectXContext, is )
		void DestroyAllContexts();
	private:
		//three type, [0], [1], [2](x), [3]
		std::vector<Ref<DirectXContext>> sm_ContextPool[4];
		//three type, [0], [1], [2](x), [3]
		std::queue<Ref<DirectXContext>> sm_AvailableContexts[4];
		std::mutex sm_ContextAllocationMutex;
	};

	class DirectXContext : public Context, public std::enable_shared_from_this<DirectXContext>
	{
		friend class ContextManager;
	public:
		DirectXContext(D3D12_COMMAND_LIST_TYPE Type);
		virtual ~DirectXContext();

		//use this function to get a context
		static DirectXContext& Begin(const std::wstring ID = L"");		

		static void DestroyAllContexts();

		//flush existing commands to the gpu but keep the context alive
		uint64_t Flush(bool WaitForCompletion);

		//flush existing commands and release the current context
		uint64_t Finish(bool WaitForCompletion);
		
		//request a new command list and a allocator
		virtual void Initialize() override;
		virtual void SwapBuffers() override;
		virtual void Reset() override;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

		void FlushResourceBarriers();

		//------Buffer Operation------
		void CopyBuffer(GpuResource& Dest, GpuResource& Src);
		void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes);

		//texture copy, 1D texture copy
		void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex);
		void CopyTextureRegion(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source, RECT& Rect);

		void TransitionResource(GpuResource& Resource, D3D12_RESOURCE_STATES NewState, bool FlushImmediate = false);
		void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate);
		//------Buffer Operation------

		static void InitializeTexture(GpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[]);
		static void InitializeBuffer(GpuBuffer& Dest, const void* Data, size_t NumBytes, size_t DestOffset = 0);
		static void InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src);

		void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes);

		//------use upload buffer to write------
		DynAlloc ReserveUploadMemory(size_t SizeInBytes);
		//------use upload buffer to write------

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr);
		void SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[]);
		void SetPipelineState(const PSO& pso);

		GraphicsContext& GetGraphicsContext();
	private:
		void BindDescriptorHeaps();

		//command list need to create from the command list manager
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCurrentAllocator;

		//command list type
		D3D12_COMMAND_LIST_TYPE m_Type;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_CurrGraphicsRootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_CurrComputeRootSingature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_CurrPipelineState;

		//reference dynamic descriptor heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CurrentDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		DynamicDescriptorHeap m_DynamicViewDescriptorHeap;//HEAP_TYPE_CBV_SRV_UAV
		DynamicDescriptorHeap m_DynamicSamplerDescriptorHeap;//HEAP_TYPE_SAMPLER

		//------resource barrier------
		D3D12_RESOURCE_BARRIER m_ResourceBarrierBuffer[16];
		uint32_t m_NumBarriersToFlush;
		//------resource barrier------

		//---current using buffer(ID3D12Resource)---
		LinearAllocator m_CpuLinearAllocator;
		LinearAllocator m_GpuLinearAllocator;
		//---current using buffer(ID3D12Resource)------

		//------set id------
		std::wstring m_ID;
		void SetID(const std::wstring& ID) { m_ID = ID; }
		//------set id------
	};
}