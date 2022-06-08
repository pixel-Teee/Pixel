#pragma once

#include "Pixel/Renderer/Context/Context.h"

#include <queue>
#include <wrl/client.h>
#include <dxgi1_4.h>

#include "Platform/DirectX/d3dx12.h"
#include "Platform/DirectX/Buffer/LinearAllocator.h"
#include "Platform/DirectX/Descriptor/DirectXDynamicDescriptorHeap.h"
#include "Pixel/Renderer/Context/ContextType.h"

struct GLFWwindow;

namespace Pixel {
	class CommandList;
	class CommandListManager;
	class DirectXSwapChain;
	class DirectXDescriptorAllocator;
	class GpuResource;
	class GpuBuffer;
	class GraphicsContext;
	class DirectXPSO;
	class ContextManager;

	class DirectXContext : public Context, public std::enable_shared_from_this<DirectXContext>
	{
		friend class DirectXContextManager;
	public:
		DirectXContext(CommandListType Type, Ref<ContextManager> pContextManager);
		virtual ~DirectXContext();	

		//flush existing commands to the gpu but keep the context alive
		virtual uint64_t Flush(bool WaitForCompletion) override;

		//flush existing commands and release the current context
		virtual uint64_t Finish(bool WaitForCompletion) override;
		
		//request a new command list and a allocator
		virtual void Initialize() override;
		virtual void SwapBuffers() override;
		virtual void Reset() override;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

		virtual void FlushResourceBarriers() override;

		//------Buffer Operation------
		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) override;
		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) override;

		//texture copy, 1D texture copy
		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex);
		void CopyTextureRegion(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source, RECT& Rect);

		void TransitionResource(GpuResource& Resource, D3D12_RESOURCE_STATES NewState, bool FlushImmediate = false);
		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate);
		//------Buffer Operation------

		void InitializeTexture(GpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[]);
		void InitializeBuffer(GpuBuffer& Dest, const void* Data, size_t NumBytes, size_t DestOffset = 0);
		void InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src);

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes);

		//------use upload buffer to write------
		DynAlloc ReserveUploadMemory(size_t SizeInBytes);
		//------use upload buffer to write------

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr);
		void SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[]);
		void SetPipelineState(const DirectXPSO& pso);

		GraphicsContext& GetGraphicsContext();

		virtual void SetID(const std::wstring& ID) override { m_ID = ID; }
	protected:
		void BindDescriptorHeaps();

		Ref<ContextManager> m_OwingContextManager;

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

		DirectXDynamicDescriptorHeap m_DynamicViewDescriptorHeap;//HEAP_TYPE_CBV_SRV_UAV
		DirectXDynamicDescriptorHeap m_DynamicSamplerDescriptorHeap;//HEAP_TYPE_SAMPLER

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
		//------set id------
	};
}