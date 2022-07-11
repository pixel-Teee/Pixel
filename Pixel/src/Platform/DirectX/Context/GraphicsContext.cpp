#include "pxpch.h"
#include "GraphicsContext.h"

#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/Buffer/DepthBuffer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/Command/CommandQueue.h"
#include "Pixel/Math/Math.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Platform/DirectX/Buffer/DirectXGpuVirtualAddress.h"
#include "Platform/DirectX/View/DirectXIndexBufferView.h"
#include "Platform/DirectX/View/DirectXVertexBufferView.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	GraphicsContext::GraphicsContext(CommandListType Type)
		:DirectXContext(Type)
	{

	}

	void GraphicsContext::ClearUAV(DirectXGpuBuffer& Target)
	{
		FlushResourceBarriers();

		Ref<DirectXDescriptorCpuHandle> UavHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Target.GetUAV());

		//after binding a uav, we can get a gpu handle that is required to clear it as a uav(because it essentially runs
		//a shader to set all of the values)
		D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibileHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(m_DynamicViewDescriptorHeap.UploadDirect(UavHandle))->GetGpuHandle();

		const uint32_t ClearColor[4] = {};

		m_pCommandList->ClearUnorderedAccessViewUint(GpuVisibileHandle, UavHandle->GetCpuHandle(), Target.GetResource(), ClearColor, 0, nullptr);
	}

	void GraphicsContext::ClearUAV(DirectXColorBuffer& Target)
	{
		FlushResourceBarriers();

		Ref<DirectXDescriptorCpuHandle> UavHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Target.GetUAV());

		//after binding a uav, we can get a gpu handle that is required to clear it as a uav(because it essentially runs
		//a shader to set all of the values)
		D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibleHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(m_DynamicViewDescriptorHeap.UploadDirect(UavHandle))->GetGpuHandle();
		CD3DX12_RECT ClearRect(0, 0, (LONG)Target.GetWidth(), (LONG)Target.GetHeight());

		//TODO: nvidia card is not clearing uavs with either float or uint variants
		const float* ClearColor = glm::value_ptr(Target.GetClearColor());
		m_pCommandList->ClearUnorderedAccessViewFloat(GpuVisibleHandle, UavHandle->GetCpuHandle(), Target.GetResource(), ClearColor, 1, &ClearRect);
	}

	void GraphicsContext::ClearColor(GpuResource& Target, PixelRect* Rect)
	{
		FlushResourceBarriers();

		DirectXColorBuffer& ColorBuffer = static_cast<DirectXColorBuffer&>(Target);

		Ref<DirectXDescriptorCpuHandle> RtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(ColorBuffer.GetRTV());

		D3D12_RECT DxRect;

		if(Rect != nullptr)
			DxRect = RectToDirectXRect(*Rect);

		m_pCommandList->ClearRenderTargetView(RtvHandle->GetCpuHandle(), glm::value_ptr(ColorBuffer.GetClearColor()), (Rect == nullptr) ? 0 : 1, &DxRect);
	}

	void GraphicsContext::ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect)
	{
		FlushResourceBarriers();

		DirectXColorBuffer& ColorBuffer = static_cast<DirectXColorBuffer&>(Target);

		Ref<DirectXDescriptorCpuHandle> RtvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(ColorBuffer.GetRTV());

		D3D12_RECT DxRect;
		if (Rect != nullptr)
			DxRect = RectToDirectXRect(*Rect);

		m_pCommandList->ClearRenderTargetView(RtvHandle->GetCpuHandle(), Color, (Rect == nullptr) ? 0 : 1, &DxRect);
	}

	void GraphicsContext::ClearDepth(GpuResource& Target)
	{
		FlushResourceBarriers();

		DepthBuffer& buffer = static_cast<DepthBuffer&>(Target);
		m_pCommandList->ClearDepthStencilView(std::static_pointer_cast<DirectXDescriptorCpuHandle>(buffer.GetDSV())->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, buffer.GetClearDepth(), buffer.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::ClearStencil(GpuResource& Target)
	{
		FlushResourceBarriers();

		DepthBuffer& buffer = static_cast<DepthBuffer&>(Target);
		m_pCommandList->ClearDepthStencilView(std::static_pointer_cast<DirectXDescriptorCpuHandle>(buffer.GetDSV())->GetCpuHandle(), D3D12_CLEAR_FLAG_STENCIL, buffer.GetClearDepth(), buffer.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::ClearDepthAndStencil(GpuResource& Target)
	{
		FlushResourceBarriers();

		DepthBuffer& buffer = static_cast<DepthBuffer&>(Target);
		m_pCommandList->ClearDepthStencilView(std::static_pointer_cast<DirectXDescriptorCpuHandle>(buffer.GetDSV())->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, buffer.GetClearDepth(), buffer.GetClearStencil(), 0, nullptr);
	}

	void GraphicsContext::SetRootSignature(const RootSignature& RootSig)
	{
		if (static_cast<DirectXRootSignature&>(const_cast<RootSignature&>(RootSig)).GetNativeSignature() == m_CurrGraphicsRootSignature.Get())
			return;

		m_CurrGraphicsRootSignature = static_cast<DirectXRootSignature&>(const_cast<RootSignature&>(RootSig)).GetComPtrSignature();
		m_pCommandList->SetGraphicsRootSignature(m_CurrGraphicsRootSignature.Get());

		m_DynamicViewDescriptorHeap.ParseGraphicsRootSignature(RootSig);
		m_DynamicSamplerDescriptorHeap.ParseGraphicsRootSignature(RootSig);
	}

	void GraphicsContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RtvHandles;
		for (uint32_t i = 0; i < RTVs.size(); ++i)
		{
			RtvHandles.push_back(std::static_pointer_cast<DirectXDescriptorCpuHandle>(RTVs[i])->GetCpuHandle());
		}

		if (RTVs.size() != 0)
			m_pCommandList->OMSetRenderTargets(NumRTVs, &RtvHandles[0], false, nullptr);
		else
			m_pCommandList->OMSetRenderTargets(NumRTVs, nullptr, false, nullptr);
	}

	void GraphicsContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV)
	{
		//m_pCommandList->OMSetRenderTargets(NumRTVs, RTVs, false, &DSV);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RtvHandles;
		D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DSV)->GetCpuHandle();

		for (uint32_t i = 0; i < RTVs.size(); ++i)
		{
			RtvHandles.push_back(std::static_pointer_cast<DirectXDescriptorCpuHandle>(RTVs[i])->GetCpuHandle());
		}

		if(RTVs.size() != 0)
			m_pCommandList->OMSetRenderTargets(NumRTVs, &RtvHandles[0], false, &DsvHandle);
		else
			m_pCommandList->OMSetRenderTargets(NumRTVs, nullptr, false, &DsvHandle);
	}

	void GraphicsContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV)
	{
		std::vector<Ref<DescriptorCpuHandle>> v;
		v.push_back(RTV);
		SetRenderTargets(1, v);
	}

	void GraphicsContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV)
	{
		std::vector<Ref<DescriptorCpuHandle>> v;
		v.push_back(RTV);
		SetRenderTargets(1, v, DSV);
	}

	void GraphicsContext::SetViewport(const ViewPort& vp)
	{
		D3D12_VIEWPORT viewPort = ViewPortToDirectXViewPort(vp);

		m_pCommandList->RSSetViewports(1, &viewPort);
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

	void GraphicsContext::SetScissor(const PixelRect& rect)
	{
		D3D12_RECT DxRect = RectToDirectXRect(rect);
		PX_CORE_ASSERT(DxRect.left < DxRect.right && DxRect.top < DxRect.bottom, "scissors's information is error!");
		m_pCommandList->RSSetScissorRects(1, &DxRect);
	}

	void GraphicsContext::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
	{
		PixelRect Rect;
		Rect.Left = left;
		Rect.Right = right;
		Rect.Top = top;
		Rect.Bottom = bottom;
		SetScissor(Rect);
	}

	void GraphicsContext::SetViewportAndScissor(const ViewPort& vp, const PixelRect& rect)
	{
		PX_CORE_ASSERT(rect.Left < rect.Right&& rect.Top < rect.Bottom, "rect's information is error!");

		D3D12_VIEWPORT ViewPort = ViewPortToDirectXViewPort(vp);
		D3D12_RECT Rect = RectToDirectXRect(rect);

		m_pCommandList->RSSetViewports(1, &ViewPort);
		m_pCommandList->RSSetScissorRects(1, &Rect);
	}

	void GraphicsContext::SetStencilRef(uint32_t StencilRef)
	{
		m_pCommandList->OMSetStencilRef(StencilRef);
	}

	void GraphicsContext::SetBlendFactor(glm::vec4 BlendFactor)
	{
		m_pCommandList->OMSetBlendFactor(glm::value_ptr(BlendFactor));
	}

	void GraphicsContext::SetPrimitiveTopology(PrimitiveTopology Topology)
	{
		D3D12_PRIMITIVE_TOPOLOGY DxPrimitiveToplogy = PrimitiveTopologyToDirectXPrimitiveTopology(Topology);
		m_pCommandList->IASetPrimitiveTopology(DxPrimitiveToplogy);
	}

	void GraphicsContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants)
	{
		m_pCommandList->SetGraphicsRoot32BitConstants(RootIndex, NumConstants, pConstants, 0);
	}

	void GraphicsContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants, uint32_t offset)
	{
		m_pCommandList->SetGraphicsRoot32BitConstants(RootIndex, NumConstants, pConstants, offset);
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

	void GraphicsContext::SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV)
	{
		Ref<DirectXGpuVirtualAddress> DxGpuVirtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(CBV);
		m_pCommandList->SetGraphicsRootConstantBufferView(RootIndex, DxGpuVirtualAddress->GetGpuVirtualAddress());
	}

	void GraphicsContext::SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is nullptr or buffer data is not aligned 16");
		//temp allocate ID3D12Resource
		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize, 256);

		memcpy(cb.DataPtr, BufferData, BufferSize);

		m_pCommandList->SetGraphicsRootConstantBufferView(RootIndex, cb.GpuAddress);
	}

	void GraphicsContext::SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset /*= 0*/)
	{
		DirectXGpuBuffer& Srv = static_cast<DirectXGpuBuffer&>(const_cast<GpuResource&>(SRV));
		PX_CORE_ASSERT((Srv.m_UsageState & (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) != 0,
			"gpu buffer's state is error!");

		Ref<DirectXGpuVirtualAddress> pVirtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(Srv.m_GpuVirtualAddress);

		m_pCommandList->SetGraphicsRootShaderResourceView(RootIndex, pVirtualAddress->GetGpuVirtualAddress() + Offset);
	}

	void GraphicsContext::SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset /*= 0*/)
	{
		DirectXGpuBuffer& Uav = static_cast<DirectXGpuBuffer&>(const_cast<GpuResource&>(UAV));

		PX_CORE_ASSERT((Uav.m_UsageState & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0, "gpu buffer's state is error!");

		Ref<DirectXGpuVirtualAddress> pVirtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(Uav.GetGpuVirtualAddress());

		m_pCommandList->SetGraphicsRootUnorderedAccessView(RootIndex, pVirtualAddress->GetGpuVirtualAddress() + Offset);
	}

	void GraphicsContext::SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(FirstHandle)->GetGpuHandle();
		m_pCommandList->SetGraphicsRootDescriptorTable(RootIndex, handle);
	}

	void GraphicsContext::SetIndexBuffer(const Ref<IBV> IBView)
	{
		Ref<DirectXIBV> DxIbv = std::static_pointer_cast<DirectXIBV>(IBView);
		m_pCommandList->IASetIndexBuffer(&DxIbv->GetIndexBufferView());
	}

	void GraphicsContext::SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView)
	{
		std::vector<Ref<VBV>> VBViews;
		VBViews.push_back(VBView);
		//Ref<DirectXVBV> DxVbv = std::static_pointer_cast<DirectXVBV>(VBView);
		SetVertexBuffers(Slot, 1, VBViews);
	}

	void GraphicsContext::SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> VertexBufferViews;
		for (uint32_t i = 0; i < VBViews.size(); ++i)
		{
			Ref<DirectXVBV> DxVBV = std::static_pointer_cast<DirectXVBV>(VBViews[i]);
			VertexBufferViews.push_back(DxVBV->GetVertexBufferView());
		}

		m_pCommandList->IASetVertexBuffers(StartSlot, Count, &VertexBufferViews[0]);
	}

	void GraphicsContext::SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VertexData)
	{
		//PX_CORE_ASSERT(VertexData != nullptr && Math::IsAligned(VertexData, 16), "vertex data is nullptr or vertex data is not aligned!");

		//temp allocate
		size_t BufferSize = Math::AlignUp(NumVertices * VertexStride, 16);
		DynAlloc vb = m_CpuLinearAllocator.Allocate(BufferSize, 256);

		memcpy(vb.DataPtr, VertexData, BufferSize);

		D3D12_VERTEX_BUFFER_VIEW VBView;
		VBView.BufferLocation = vb.GpuAddress;
		VBView.SizeInBytes = (uint32_t)BufferSize;
		VBView.StrideInBytes = (uint32_t)VertexStride;

		m_pCommandList->IASetVertexBuffers(Slot, 1, &VBView);
	}

	void GraphicsContext::SetDynamicIB(size_t IndexCount, const uint16_t* IBData)
	{
		//PX_CORE_ASSERT(IBData != nullptr && Math::IsAligned(IBData, 16), "IndexData is nullptr or IndexData is not aligned 16!");

		size_t BufferSize = Math::AlignUp(IndexCount * sizeof(uint16_t), 16);
		DynAlloc ib = m_CpuLinearAllocator.Allocate(BufferSize, 256);

		memcpy(ib.DataPtr, IBData, BufferSize);

		D3D12_INDEX_BUFFER_VIEW IBView;
		IBView.BufferLocation = ib.GpuAddress;
		IBView.SizeInBytes = (uint32_t)(IndexCount * sizeof(uint16_t));
		IBView.Format = DXGI_FORMAT_R16_UINT;

		m_pCommandList->IASetIndexBuffer(&IBView);
	}

	void GraphicsContext::SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is null ptr and is not aligned");

		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize, 256);
		memcpy(cb.DataPtr, BufferData, Math::AlignUp(BufferSize, 16));

		m_pCommandList->SetGraphicsRootShaderResourceView(RootIndex, cb.GpuAddress);
	}

	void GraphicsContext::Draw(uint32_t VertexCount, uint32_t VertexStartOffset /*= 0*/)
	{

	}

	void GraphicsContext::DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation /*= 0*/, int32_t BaseVertexLocation /*= 0*/)
	{
		DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
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

	void GraphicsContext::WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}