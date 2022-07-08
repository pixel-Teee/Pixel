#pragma once

#include "glm/gtc/type_ptr.hpp"

#include "DirectXContext.h"

namespace Pixel {
	class DirectXColorBuffer;
	class DepthBuffer;
	class GpuResource;
	class Device;
	class GraphicsContext : public DirectXContext
	{
	public:
		GraphicsContext(CommandListType Type);
		void ClearUAV(DirectXGpuBuffer& Target);//TODO:need to add interface
		void ClearUAV(DirectXColorBuffer& Target);//TODO:need to add interface
		virtual void ClearColor(GpuResource& Target, PixelRect* Rect = nullptr) override;
		virtual void ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect = nullptr) override;
		void ClearDepth(GpuResource& Target);
		void ClearStencil(GpuResource& Target);
		void ClearDepthAndStencil(GpuResource& Target);

		virtual void SetRootSignature(const RootSignature& RootSig) override;

		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs) override;
		virtual void SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV) override;
		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV) override;
		virtual void SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV) override;
		virtual void SetDepthStencilTarget(Ref<DescriptorCpuHandle> DSV) override { std::vector<Ref<DescriptorCpuHandle>> v; SetRenderTargets(0, v, DSV); }

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
		virtual void SetConstants(uint32_t RootIndex, uint32_t x)  override;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y)  override;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z)  override;
		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w)  override;
		virtual void SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV)  override;
		virtual void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)  override;
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
		virtual void DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation = 0,
			uint32_t StartInstanceLocation = 0) override;
		virtual void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation,
			int32_t BaseVertexLocation, uint32_t StartInstanceLocation) override;

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes) override;

	};
}
