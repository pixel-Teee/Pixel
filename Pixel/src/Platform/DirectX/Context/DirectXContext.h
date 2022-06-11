#pragma once

#include "Pixel/Renderer/Context/Context.h"

#include <vector>
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
	class IBV;
	class VBV;
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;
	class DescriptorHeap;

	class DirectXContext : public Context
	{
		friend class DirectXContextManager;
	public:
		DirectXContext(CommandListType Type, Ref<ContextManager> pContextManager, Ref<Device> pDevice);
		virtual ~DirectXContext();	

		//flush existing commands to the gpu but keep the context alive
		virtual uint64_t Flush(bool WaitForCompletion, Ref<Device> pDevice) override;

		//flush existing commands and release the current context
		virtual uint64_t Finish(bool WaitForCompletion, Ref<ContextManager> contextManager, Ref<Device> pDevice) override;
		
		//request a new command list and a allocator
		virtual void Initialize(Ref<Device> pDevice) override;
		virtual void SwapBuffers() override;
		virtual void Reset(Ref<Device> pDevice) override;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

		//ID3D12GraphicsCommandList
		virtual void* GetNativeCommandList() override;

		virtual void FlushResourceBarriers() override;

		//------Buffer Operation------
		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) override;
		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) override;

		//texture copy, 1D texture copy
		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex);
		void CopyTextureRegion(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source, RECT& Rect);

		virtual void TransitionResource(GpuResource& Resource, ResourceStates NewState, bool FlushImmediate = false) override;
		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate);
		//------Buffer Operation------

		void InitializeTexture(GpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[], Ref<ContextManager> m_pContextManager, Ref<Device> pDevice);
		void InitializeBuffer(GpuBuffer& Dest, const void* Data, size_t NumBytes, size_t DestOffset, Ref<ContextManager> m_pContextManager, Ref<Device> pDevice);
		void InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src, Ref<ContextManager> m_pContextManager, Ref<Device> pDevice);

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes, Ref<Device> pDevice);

		//------use upload buffer to write------
		DynAlloc ReserveUploadMemory(size_t SizeInBytes, Ref<Device> pDevice);
		//------use upload buffer to write------

		virtual void SetDescriptorHeap(DescriptorHeapType Type, Ref<DescriptorHeap> HeapPtr) override;
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, ID3D12DescriptorHeap* HeapPtr);
		void SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[]);
		void SetPipelineState(const DirectXPSO& pso);

		GraphicsContext& GetGraphicsContext();

		virtual void SetID(const std::wstring& ID) override { m_ID = ID; }

		//TODO:need to clear these functions
		virtual void ClearColor(PixelBuffer& Target, PixelRect* Rect) override;

		virtual void ClearColor(PixelBuffer& Target, float Color[4], PixelRect* Rect = nullptr) override;

		virtual void ClearDepth(PixelBuffer& Target) override;

		virtual void ClearStencil(PixelBuffer& Target) override;

		virtual void ClearDepthAndStencil(PixelBuffer& Target) override;

		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs) override;

		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV) override;

		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV) override;

		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV) override;

		virtual void SetDepthStencilTarget(Ref<DescriptorCpuHandle> DSV) override;

		virtual void SetRootSignature(const RootSignature& RootSig) override;

		virtual void SetViewport(const ViewPort& vp) override;

		virtual void SetViewport(float x, float y, float w, float h, float minDepth = 0.0f, float maxDepth = 1.0f) override;

		virtual void SetScissor(const PixelRect& rect) override;

		virtual void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) override;

		virtual void SetViewportAndScissor(const ViewPort& vp, const PixelRect& rect) override;

		virtual void SetStencilRef(uint32_t StencilRef) override;

		virtual void SetBlendFactor(glm::vec4 BlendFactor) override;

		virtual void SetPrimitiveTopology(PrimitiveTopology Topology) override;

		virtual void SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants) override;

		virtual void SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w) override;

		virtual void SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV) override;

		virtual void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData, Ref<Device> pDevice) override;

		virtual void SetBufferSRV(uint32_t RootIndex, const GpuBuffer& SRV, uint64_t Offset = 0) override;

		virtual void SetBufferUAV(uint32_t RootIndex, const GpuBuffer& UAV, uint64_t Offset = 0) override;

		virtual void SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle) override;

		virtual void SetIndexBuffer(const Ref<IBV> IBView) override;

		virtual void SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView) override;

		virtual void SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews) override;

		virtual void SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData, Ref<Device> pDevice) override;

		virtual void SetDynamicIB(size_t IndexCount, const uint64_t* IBData, Ref<Device> pDevice) override;

		virtual void SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData, Ref<Device> pDevice) override;

		virtual void Draw(uint32_t VertexCount, uint32_t VertexStartOffset = 0) override;

		virtual void DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation = 0, int32_t BaseVertexLocation = 0) override;

		virtual void DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation = 0, uint32_t StartInstanceLocation = 0) override;

		virtual void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation) override;

		virtual CommandListType GetType() override;

		//TODO:need to clear these functions

	protected:
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