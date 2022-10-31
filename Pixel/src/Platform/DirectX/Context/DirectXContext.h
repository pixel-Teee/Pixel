#pragma once

#include <vector>
#include <queue>

#include <wrl/client.h>
#include <dxgi1_4.h>
#include <glm/glm.hpp>

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Context/Context.h"
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
	class GraphicsContext;
	class DirectXPSO;
	class ContextManager;
	class IBV;
	class VBV;
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;
	class DescriptorHeap;
	class PSO;

	class DirectXContext : public Context
	{
		friend class DirectXContextManager;
	public:
		DirectXContext(CommandListType Type);
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

		//ID3D12GraphicsCommandList
		virtual void* GetNativeCommandList() override;

		virtual void FlushResourceBarriers() override;

		//------Buffer Operation------
		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) override;
		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) override;

		//texture copy, 1D texture copy
		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex);
		void CopyTextureRegion(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source, RECT& Rect);

		void CopyTextureToBuffer(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source);

		virtual void TransitionResource(GpuResource& Resource, ResourceStates NewState, bool FlushImmediate = false) override;
		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate);
		//------Buffer Operation------

		void InitializeTexture(GpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[]);
		void InitializeBuffer(GpuResource& Dest, const void* Data, size_t NumBytes, size_t DestOffset);
		void InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src);

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes);

		//------use upload buffer to write------
		DynAlloc ReserveUploadMemory(size_t SizeInBytes);
		//------use upload buffer to write------

		virtual void SetDescriptorHeap(DescriptorHeapType Type, Ref<DescriptorHeap> HeapPtr) override;
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, ID3D12DescriptorHeap* HeapPtr);
		void SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[]);
		virtual void SetPipelineState(const PSO& pso) override;

		GraphicsContext& GetGraphicsContext();

		virtual void SetID(const std::wstring& ID) override { m_ID = ID; }

		//TODO:need to clear these functions
		virtual void ClearColor(GpuResource& Target, PixelRect* Rect) override;

		virtual void ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect = nullptr) override;

		virtual void ClearDepth(GpuResource& Target) override;

		virtual void ClearStencil(GpuResource& Target) override;

		virtual void ClearDepthAndStencil(GpuResource& Target) override;

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

		virtual void SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants, uint32_t offset) override;

		virtual void SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w) override;

		virtual void SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV) override;

		virtual void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData) override;

		virtual void SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset = 0) override;

		virtual void SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset = 0) override;

		virtual void SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle) override;

		virtual void SetIndexBuffer(const Ref<IBV> IBView) override;

		virtual void SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView) override;

		virtual void SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews) override;

		virtual void SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData) override;

		virtual void SetDynamicIB(size_t IndexCount, const uint16_t* IBData) override;

		virtual void SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData) override;

		virtual void Draw(uint32_t VertexCount, uint32_t VertexStartOffset = 0) override;

		virtual void DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation = 0, int32_t BaseVertexLocation = 0) override;

		virtual void DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation = 0, uint32_t StartInstanceLocation = 0) override;

		virtual void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation) override;

		virtual CommandListType GetType() override;

		//------Compute Context Function------
		virtual void Dispatch(size_t GroupCountX = 1, size_t GroupCountY = 1, size_t GroupCountZ = 1) override;

		virtual void Dispatch1D(size_t ThreadCountX, size_t GroupSizeX = 64) override;

		virtual void Dispatch2D(size_t ThreadCountX, size_t ThreadCountY, size_t GroupSizeX = 8, size_t GroupSizeY = 8) override;
		//------Compute Context Function------
		//TODO:need to clear these functions
		virtual void SetType(CommandListType type) override;
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