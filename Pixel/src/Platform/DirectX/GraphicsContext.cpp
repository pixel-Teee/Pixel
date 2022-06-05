#include "pxpch.h"
#include "GraphicsContext.h"

#include "DirectXBuffer.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "DirectXRootSignature.h"
#include "CommandQueue.h"
#include "Pixel/Math/Math.h"

namespace Pixel {

	GraphicsContext& GraphicsContext::Begin(const std::wstring& ID)
	{
		return DirectXContext::Begin(ID).GetGraphicsContext();
	}

	void GraphicsContext::ClearUAV(GpuBuffer& Target)
	{
		FlushResourceBarriers();

		//after binding a uav, we can get a gpu handle that is required to clear it as a uav(because it essentially runs
		//a shader to set all of the values)
		D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibileHandle = m_DynamicViewDescriptorHeap.UploadDirect(Target.GetUAV());

		const uint32_t ClearColor[4] = {};

		m_pCommandList->ClearUnorderedAccessViewUint(GpuVisibileHandle, Target.GetUAV(), Target.GetResource(), ClearColor, 0, nullptr);
	}

	void GraphicsContext::ClearUAV(ColorBuffer& Target)
	{
		FlushResourceBarriers();

		//after binding a uav, we can get a gpu handle that is required to clear it as a uav(because it essentially runs
		//a shader to set all of the values)
		D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibleHandle = m_DynamicViewDescriptorHeap.UploadDirect(Target.GetUAV());
		CD3DX12_RECT ClearRect(0, 0, (LONG)Target.GetWidth(), (LONG)Target.GetHeight());

		//TODO: nvidia card is not clearing uavs with either float or uint variants
		const float* ClearColor = glm::value_ptr(Target.GetClearColor());
		m_pCommandList->ClearUnorderedAccessViewFloat(GpuVisibleHandle, Target.GetUAV(), Target.GetResource(), ClearColor, 1, &ClearRect);
	}

	void GraphicsContext::ClearColor(ColorBuffer& Target, D3D12_RECT* Rect /*= nullptr*/)
	{
		FlushResourceBarriers();
		m_pCommandList->ClearRenderTargetView(Target.GetRTV(), glm::value_ptr(Target.GetClearColor()), (Rect == nullptr) ? 0 : 1, Rect);
	}

	void GraphicsContext::ClearColor(ColorBuffer& Target, float Color[4], D3D12_RECT* Rect /*= nullptr*/)
	{
		FlushResourceBarriers();
		m_pCommandList->ClearRenderTargetView(Target.GetRTV(), Color, (Rect == nullptr) ? 0 : 1, Rect);
	}

	void GraphicsContext::ClearDepth(DepthBuffer& Target)
	{
		FlushResourceBarriers();
		m_pCommandList->ClearDepthStencilView(Target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, Target.GetClearDepth(), Target.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::ClearStencil(DepthBuffer& Target)
	{
		FlushResourceBarriers();
		m_pCommandList->ClearDepthStencilView(Target.GetDSV(), D3D12_CLEAR_FLAG_STENCIL, Target.GetClearDepth(), Target.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::ClearDepthAndStencil(DepthBuffer& Target)
	{
		FlushResourceBarriers();
		m_pCommandList->ClearDepthStencilView(Target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, Target.GetClearDepth(), Target.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::SetRootSignature(const RootSignature& RootSig)
	{
		if (RootSig.GetNativeSignature() == m_CurrGraphicsRootSignature.Get())
			return;

		m_CurrGraphicsRootSignature.Attach(RootSig.GetNativeSignature());
		m_pCommandList->SetGraphicsRootSignature(m_CurrGraphicsRootSignature.Get());

		m_DynamicViewDescriptorHeap.ParseGraphicsRootSignature(RootSig);
		m_DynamicSamplerDescriptorHeap.ParseGraphicsRootSignature(RootSig);
	}

	void GraphicsContext::SetRenderTargets(uint32_t NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTVs[])
	{
		m_pCommandList->OMSetRenderTargets(NumRTVs, RTVs, false, nullptr);
	}

	void GraphicsContext::SetRenderTargets(uint32_t NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTVs[], D3D12_CPU_DESCRIPTOR_HANDLE DSV)
	{
		m_pCommandList->OMSetRenderTargets(NumRTVs, RTVs, false, &DSV);
	}

	void GraphicsContext::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV)
	{
		SetRenderTargets(1, &RTV);
	}

	void GraphicsContext::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV, D3D12_CPU_DESCRIPTOR_HANDLE DSV)
	{
		SetRenderTargets(1, &RTV, DSV);
	}

	void GraphicsContext::SetViewport(const D3D12_VIEWPORT& vp)
	{
		m_pCommandList->RSSetViewports(1, &vp);
	}

	void GraphicsContext::SetViewport(float x, float y, float w, float h, float minDepth /*= 0.0f*/, float maxDepth /*= 1.0f*/)
	{
		D3D12_VIEWPORT vp;
		vp.Width = w;
		vp.Height = h;
		vp.MinDepth = minDepth;
		vp.MaxDepth = maxDepth;
		vp.TopLeftX = x;
		vp.TopLeftY = y;
		m_pCommandList->RSSetViewports(1, &vp);
	}

	void GraphicsContext::SetScissor(const D3D12_RECT& rect)
	{
		PX_CORE_ASSERT(rect.left < rect.right&& rect.top < rect.bottom, "scissors's information is error!");
		m_pCommandList->RSSetScissorRects(1, &rect);
	}

	void GraphicsContext::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
	{
		SetScissor(CD3DX12_RECT(left, top, right, bottom));
	}

	void GraphicsContext::SetViewportAndScissor(const D3D12_VIEWPORT& vp, const D3D12_RECT& rect)
	{
		PX_CORE_ASSERT(rect.left < rect.right&& rect.top < rect.bottom, "rect's information is error!");

		m_pCommandList->RSSetViewports(1, &vp);
		m_pCommandList->RSSetScissorRects(1, &rect);
	}

	void GraphicsContext::SetStencilRef(uint32_t StencilRef)
	{
		m_pCommandList->OMSetStencilRef(StencilRef);
	}

	void GraphicsContext::SetBlendFactor(glm::vec4 BlendFactor)
	{
		m_pCommandList->OMSetBlendFactor(glm::value_ptr(BlendFactor));
	}

	void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY Topology)
	{
		m_pCommandList->IASetPrimitiveTopology(Topology);
	}

	void GraphicsContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants)
	{
		m_pCommandList->SetGraphicsRoot32BitConstants(RootIndex, NumConstants, pConstants, 0);
	}

	void GraphicsContext::SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val)
	{
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, Val, Offset);
	}

	void GraphicsContext::SetConstants(uint32_t RootIndex, uint32_t x)
	{
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, x, 0);
	}

	void GraphicsContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y)
	{
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, y, 1);
	}

	void GraphicsContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z)
	{
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, y, 1);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, z, 2);
	}

	void GraphicsContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
	{
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, y, 1);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, z, 2);
		m_pCommandList->SetGraphicsRoot32BitConstant(RootIndex, w, 3);
	}

	void GraphicsContext::SetConstantBuffer(uint32_t RootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV)
	{
		m_pCommandList->SetGraphicsRootConstantBufferView(RootIndex, CBV);
	}

	void GraphicsContext::SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is nullptr or buffer data is not aligned 16");
		//temp allocate ID3D12Resource
		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize);

		memcpy(cb.DataPtr, BufferData, BufferSize);

		m_pCommandList->SetGraphicsRootConstantBufferView(RootIndex, cb.GpuAddress);
	}

	void GraphicsContext::SetBufferSRV(uint32_t RootIndex, const GpuBuffer& SRV, uint64_t Offset /*= 0*/)
	{
		PX_CORE_ASSERT((SRV.m_UsageState & (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) != 0,
			"gpu buffer's state is error!");

		m_pCommandList->SetGraphicsRootShaderResourceView(RootIndex, SRV.GetGpuVirtualAddress() + Offset);
	}

	void GraphicsContext::SetBufferUAV(uint32_t RootIndex, const GpuBuffer& UAV, uint64_t Offset /*= 0*/)
	{
		PX_CORE_ASSERT((UAV.m_UsageState & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0, "gpu buffer's state is error!");
		m_pCommandList->SetGraphicsRootUnorderedAccessView(RootIndex, UAV.GetGpuVirtualAddress() + Offset);
	}

	void GraphicsContext::SetDescriptorTable(uint32_t RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle)
	{
		m_pCommandList->SetGraphicsRootDescriptorTable(RootIndex, FirstHandle);
	}

	void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& IBView)
	{
		m_pCommandList->IASetIndexBuffer(&IBView);
	}

	void GraphicsContext::SetVertexBuffer(uint32_t Slot, const D3D12_VERTEX_BUFFER_VIEW& VBView)
	{
		SetVertexBuffers(Slot, 1, &VBView);
	}

	void GraphicsContext::SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const D3D12_VERTEX_BUFFER_VIEW VBViews[])
	{
		m_pCommandList->IASetVertexBuffers(StartSlot, Count, VBViews);
	}

	void GraphicsContext::SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VertexData)
	{
		PX_CORE_ASSERT(VertexData != nullptr && Math::IsAligned(VertexData, 16), "vertex data is nullptr or vertex data is not aligned!");

		//temp allocate
		size_t BufferSize = Math::AlignUp(NumVertices * VertexStride, 16);
		DynAlloc vb = m_CpuLinearAllocator.Allocate(BufferSize);

		memcpy(vb.DataPtr, VertexData, BufferSize);

		D3D12_VERTEX_BUFFER_VIEW VBView;
		VBView.BufferLocation = vb.GpuAddress;
		VBView.SizeInBytes = (uint32_t)BufferSize;
		VBView.StrideInBytes = (uint32_t)VertexStride;

		m_pCommandList->IASetVertexBuffers(Slot, 1, &VBView);
	}

	void GraphicsContext::SetDynamicIB(size_t IndexCount, const uint64_t* IndexData)
	{
		PX_CORE_ASSERT(IndexData != nullptr && Math::IsAligned(IndexData, 16), "IndexData is nullptr or IndexData is not aligned 16!");

		size_t BufferSize = Math::AlignUp(IndexCount * sizeof(uint16_t), 16);
		DynAlloc ib = m_CpuLinearAllocator.Allocate(BufferSize);

		memcpy(ib.DataPtr, IndexData, BufferSize);

		D3D12_INDEX_BUFFER_VIEW IBView;
		IBView.BufferLocation = ib.GpuAddress;
		IBView.SizeInBytes = (uint32_t)(IndexCount * sizeof(uint16_t));
		IBView.Format = DXGI_FORMAT_R16_UINT;

		m_pCommandList->IASetIndexBuffer(&IBView);
	}

	void GraphicsContext::SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is null ptr and is not aligned");

		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize);
		memcpy(cb.DataPtr, BufferData, Math::AlignUp(BufferSize, 16));

		m_pCommandList->SetGraphicsRootShaderResourceView(RootIndex, cb.GpuAddress);
	}

	void GraphicsContext::Draw(uint32_t VertexCount, uint32_t VertexStartOffset /*= 0*/)
	{

	}

	void GraphicsContext::DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation /*= 0*/, int32_t BaseVertexLocation /*= 0*/)
	{
		
	}

	void GraphicsContext::DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation /*= 0*/,
	uint32_t StartInstanceLocation /*= 0*/)
	{
		FlushResourceBarriers();
		m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pCommandList.Get());
		m_DynamicSamplerDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pCommandList.Get());
		m_pCommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}

	void GraphicsContext::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation,
		int32_t BaseVertexLocation, uint32_t StartInstanceLocation)
	{
		FlushResourceBarriers();
		m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pCommandList.Get());
		m_DynamicSamplerDescriptorHeap.CommitComputeRootDescriptorTables(m_pCommandList.Get());
		m_pCommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StatrIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

}