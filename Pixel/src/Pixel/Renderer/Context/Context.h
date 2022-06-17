#pragma once

#include <glm/glm.hpp>

#include "Pixel/Renderer/RendererType.h"
#include "Pixel/Renderer/Descriptor/HeapType.h"
#include "Pixel/Renderer/Context/ContextType.h"

namespace Pixel {

	class RootSignature;
	class DescriptorCpuHandle;
	class DescriptorGpuHandle;
	class GpuVirtualAddress;
	class IBV;
	class VBV;
	class DescriptorHeap;
	class ContextManager;
	class GpuResource;
	class PSO;

	class Context : public std::enable_shared_from_this<Context>
	{
	public:
		virtual ~Context();

		virtual void Initialize() = 0;

		virtual void* GetNativeCommandList() = 0;

		virtual void Reset() = 0;

		virtual CommandListType GetType() = 0;

		//TODO:refractor this
		virtual void SwapBuffers() = 0;
		
		virtual void SetID(const std::wstring& ID) = 0;

		virtual void FlushResourceBarriers() = 0;

		//flush existing commands to the gpu but keep the context alive
		virtual uint64_t Flush(bool WaitForCompletion) = 0;

		//flush existing commands and release the current context
		virtual uint64_t Finish(bool WaitForCompletion) = 0;

		//------Buffer Operation------
		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) = 0;
		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) = 0;
		virtual void TransitionResource(GpuResource& Resource, ResourceStates NewState, bool FlushImmediate = false) = 0;

		//texture copy, 1D texture copy
		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex) = 0;
		
		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate) = 0;
		//------Buffer Operation------

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes) = 0;

		//------Graphics Pso Operation------

		virtual void SetRootSignature(const RootSignature& RootSig) = 0;
		virtual void SetDescriptorHeap(DescriptorHeapType Type, Ref<DescriptorHeap> HeapPtr) = 0;

		virtual void ClearColor(GpuResource& Target, PixelRect* Rect) = 0;
		virtual void ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect = nullptr) = 0;
		virtual void ClearDepth(GpuResource& Target) = 0;
		virtual void ClearStencil(GpuResource& Target) = 0;
		virtual void ClearDepthAndStencil(GpuResource& Target) = 0;

		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs) = 0;
		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV) = 0;
		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV) = 0;
		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV) = 0;
		virtual void SetDepthStencilTarget(Ref<DescriptorCpuHandle> DSV) = 0;
		virtual void SetViewport(const ViewPort& vp) = 0;
		virtual void SetViewport(float x, float y, float w, float h, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;
		virtual void SetScissor(const PixelRect& rect) = 0;
		virtual void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) = 0;
		virtual void SetViewportAndScissor(const ViewPort& vp, const PixelRect& rect) = 0;
		virtual void SetStencilRef(uint32_t StencilRef) = 0;
		virtual void SetBlendFactor(glm::vec4 BlendFactor) = 0;
		virtual void SetPrimitiveTopology(PrimitiveTopology Topology) = 0;

		virtual void SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants) = 0;
		virtual void SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val) = 0;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x) = 0;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y) = 0;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z) = 0;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w) = 0;
		virtual void SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV) = 0;
		virtual void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData) = 0;
		virtual void SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset = 0) = 0;
		virtual void SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset = 0) = 0;
		virtual void SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle) = 0;

		virtual void SetIndexBuffer(const Ref<IBV> IBView) = 0;
		virtual void SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView) = 0;
		virtual void SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews) = 0;
		virtual void SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData) = 0;
		virtual void SetDynamicIB(size_t IndexCount, const uint64_t* IBData) = 0;
		virtual void SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData) = 0;

		virtual void SetPipelineState(const PSO& pso) = 0;

		virtual void Draw(uint32_t VertexCount, uint32_t VertexStartOffset = 0) = 0;
		virtual void DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation = 0, int32_t BaseVertexLocation = 0) = 0;
		virtual void DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation = 0,
			uint32_t StartInstanceLocation = 0) = 0;
		virtual void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation,
			int32_t BaseVertexLocation, uint32_t StartInstanceLocation) = 0;
		//------Graphics Pso Operation------
	};

}
