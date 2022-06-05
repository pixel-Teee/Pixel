#pragma once

#include "glm/gtc/type_ptr.hpp"

#include "DirectXContext.h"

namespace Pixel {
	class ColorBuffer;
	class DepthBuffer;
	class GraphicsContext : public DirectXContext
	{
	public:

		static GraphicsContext& Begin(const std::wstring& ID = L"");

		void ClearUAV(GpuBuffer& Target);
		void ClearUAV(ColorBuffer& Target);
		void ClearColor(ColorBuffer& Target, D3D12_RECT* Rect = nullptr);
		void ClearColor(ColorBuffer& Target, float Color[4], D3D12_RECT* Rect = nullptr);
		void ClearDepth(DepthBuffer& Target);
		void ClearStencil(DepthBuffer& Target);
		void ClearDepthAndStencil(DepthBuffer& Target);

		void SetRootSignature(const RootSignature& RootSig);

		void SetRenderTargets(uint32_t NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTVs[]);
		void SetRenderTargets(uint32_t NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTVs[], D3D12_CPU_DESCRIPTOR_HANDLE DSV);
		void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV);
		void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV, D3D12_CPU_DESCRIPTOR_HANDLE DSV);
		void SetDepthStencilTarget(D3D12_CPU_DESCRIPTOR_HANDLE DSV) { SetRenderTargets(0, nullptr, DSV); }

		void SetViewport(const D3D12_VIEWPORT& vp);
		void SetViewport(float x, float y, float w, float h, float minDepth = 0.0f, float maxDepth = 1.0f);
		void SetScissor(const D3D12_RECT& rect);
		void SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
		void SetViewportAndScissor(const D3D12_VIEWPORT& vp, const D3D12_RECT& rect);
		void SetStencilRef(uint32_t StencilRef);
		void SetBlendFactor(glm::vec4 BlendFactor);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY Topology);

		void SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants);
		void SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val);
		void SetConstants(uint32_t RootIndex, uint32_t x);
		void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y);
		void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z);
		void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w);
		void SetConstantBuffer(uint32_t RootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV);
		void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData);
		void SetBufferSRV(uint32_t RootIndex, const GpuBuffer& SRV, uint64_t Offset = 0);
		void SetBufferUAV(uint32_t RootIndex, const GpuBuffer& UAV, uint64_t Offset = 0);
		void SetDescriptorTable(uint32_t RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle);

		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& IBView);
		void SetVertexBuffer(uint32_t Slot, const D3D12_VERTEX_BUFFER_VIEW& VBView);
		void SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const D3D12_VERTEX_BUFFER_VIEW VBViews[]);
		void SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData);
		void SetDynamicIB(size_t IndexCount, const uint64_t* IBData);
		void SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData);

		void Draw(uint32_t VertexCount, uint32_t VertexStartOffset = 0);
		void DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation = 0, int32_t BaseVertexLocation = 0);
		void DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation = 0,
			uint32_t StartInstanceLocation = 0);
		void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation,
			int32_t BaseVertexLocation, uint32_t StartInstanceLocation);
	};
}
