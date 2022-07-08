#include "pxpch.h"

#include "DirectXContext.h"
#include "DirectXContextManager.h"
#include "Platform/DirectX/Command/CommandQueue.h"
#include "Platform/DirectX/DirectXSwapChain.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"
#include "Platform/DirectX/View/DirectXIndexBufferView.h"
#include "Platform/DirectX/View/DirectXVertexBufferView.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Pixel/Math/Math.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorHeap.h"
#include "Platform/DirectX/Buffer/DirectXGpuResource.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace Pixel {

//------a marco------
#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
    ( D3D12_RESOURCE_STATE_UNORDERED_ACCESS \
    | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE \
    | D3D12_RESOURCE_STATE_COPY_DEST \
    | D3D12_RESOURCE_STATE_COPY_SOURCE )
//-------a marco------

	//-------DirectX Context------
	DirectXContext::DirectXContext(CommandListType Type)
		:m_Type(CmdListTypeToDirectXCmdListType(Type)),
		m_CpuLinearAllocator(kCpuWritable),
		m_GpuLinearAllocator(kGpuExclusive),
		m_DynamicViewDescriptorHeap(*this, DescriptorHeapType::CBV_UAV_SRV),
		m_DynamicSamplerDescriptorHeap(*this, DescriptorHeapType::SAMPLER)
	{
		m_NumBarriersToFlush = 0;
	}

	DirectXContext::~DirectXContext()
	{
		m_CpuLinearAllocator.DestroyAll();
		m_GpuLinearAllocator.DestroyAll();
	}

	void DirectXContext::Initialize()
	{
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->CreateNewCommandList(m_Type, m_pCommandList, m_pCurrentAllocator);
	}

	void DirectXContext::SwapBuffers()
	{
	
	}

	uint64_t DirectXContext::Flush(bool WaitForCompletion)
	{
		FlushResourceBarriers();

		uint64_t FenceValue = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->GetQueue(m_Type).ExecuteCommandList(m_pCommandList);

		if (WaitForCompletion)
			std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->WaitForFence(FenceValue);

		//reset command list and restore previous state
		m_pCommandList->Reset(m_pCurrentAllocator.Get(), nullptr);

		if (m_CurrGraphicsRootSignature)
		{
			m_pCommandList->SetGraphicsRootSignature(m_CurrGraphicsRootSignature.Get());
		}

		if (m_CurrComputeRootSingature)
		{
			m_pCommandList->SetComputeRootSignature(m_CurrComputeRootSingature.Get());
		}

		if (m_CurrPipelineState)
		{
			m_pCommandList->SetPipelineState(m_CurrPipelineState.Get());
		}

		//Bind Descriptors
		BindDescriptorHeaps();

		return FenceValue;
	}

	uint64_t DirectXContext::Finish(bool WaitForCompletion)
	{
		FlushResourceBarriers();

		CommandQueue& Queue = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->GetQueue(m_Type);

		uint64_t FenceValue = Queue.ExecuteCommandList(m_pCommandList);
		Queue.DiscardAlloactor(FenceValue, m_pCurrentAllocator);

		m_pCurrentAllocator = nullptr;
		m_CpuLinearAllocator.CleanupUsedPages(FenceValue);
		m_GpuLinearAllocator.CleanupUsedPages(FenceValue);
		m_DynamicSamplerDescriptorHeap.CleanupUsedHeaps(FenceValue);
		m_DynamicSamplerDescriptorHeap.CleanupUsedHeaps(FenceValue);
		//release and call the instance's release function
		//m_pCurrentAllocator->Reset();

		if (WaitForCompletion)
			std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->WaitForFence(FenceValue);

		//g_ContextManager.FreeContext(shared_from_this());
		//DirectXDevice::Get()->FreeContext(shared_from_this());
		DirectXDevice::Get()->GetContextManager()->FreeContext(shared_from_this());
		return FenceValue;
	}

	void DirectXContext::FlushResourceBarriers()
	{
		if (m_NumBarriersToFlush > 0)
		{
			//transition resource barrier
			m_pCommandList->ResourceBarrier(m_NumBarriersToFlush, m_ResourceBarrierBuffer);
			m_NumBarriersToFlush = 0;
		}
	}

	void DirectXContext::BindDescriptorHeaps()
	{
		uint32_t NonNullHeaps = 0;

		ID3D12DescriptorHeap* HeapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			ID3D12DescriptorHeap* HeapIter = m_CurrentDescriptorHeaps[i].Get();
			if (HeapIter != nullptr)
				HeapsToBind[NonNullHeaps++] = HeapIter;
		}

		if (NonNullHeaps > 0)
			m_pCommandList->SetDescriptorHeaps(NonNullHeaps, HeapsToBind);
	}

	void DirectXContext::CopyBuffer(GpuResource& Dest, GpuResource& Src)
	{
		TransitionResource(Dest, ResourceStates::CopyDest);
		TransitionResource(Src, ResourceStates::CopySource);
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Src);

		//------Copy Resource------
		m_pCommandList->CopyResource(DestResource.GetResource(), SrcResource.GetResource());
		//------Copy Resource------
	}

	void DirectXContext::CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes)
	{
		TransitionResource(Dest, ResourceStates::CopyDest);
		TransitionResource(Src, ResourceStates::CopySource);
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Src);

		m_pCommandList->CopyBufferRegion(DestResource.GetResource(), DestOffset, SrcResource.GetResource(), SrcOffset, NumBytes);
	}

	void DirectXContext::CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex)
	{
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Src);

		D3D12_TEXTURE_COPY_LOCATION DestLocation =
		{
			DestResource.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			DestSubIndex
		};

		//subresource index:specifies the index of the subresouce of an arrayed, mip-mapped, or planar texture should be used fo the copy operation
		D3D12_TEXTURE_COPY_LOCATION SrcLocation =
		{
			SrcResource.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			SrcSubIndex
		};

		//use gpu to copy texture data between two locations
		m_pCommandList->CopyTextureRegion(&DestLocation, 0, 0, 0, &SrcLocation, nullptr);
	}

	void DirectXContext::CopyTextureRegion(GpuResource& Dest, uint32_t x, uint32_t y, uint32_t z, GpuResource& Source, RECT& Rect)
	{
		TransitionResource(Dest, ResourceStates::CopyDest);
		TransitionResource(Source, ResourceStates::CopySource);
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Source);

		//0:subresource index
		D3D12_TEXTURE_COPY_LOCATION destLoc = CD3DX12_TEXTURE_COPY_LOCATION(DestResource.GetResource(), 0);
		D3D12_TEXTURE_COPY_LOCATION srcLoc = CD3DX12_TEXTURE_COPY_LOCATION(SrcResource.GetResource(), 0);

		D3D12_BOX box = {};
		box.back = 1;//z position of box
		box.left = Rect.left;
		box.right = Rect.right;
		box.top = Rect.top;
		box.bottom = Rect.bottom;

		m_pCommandList->CopyTextureRegion(&destLoc, x, y, z, &srcLoc, &box);
	}

	void DirectXContext::TransitionResource(GpuResource& Resource, ResourceStates State, bool FlushImmediate)
	{
		DirectXGpuResource& DirectXResource = static_cast<DirectXGpuResource&>(Resource);
		D3D12_RESOURCE_STATES OldState = DirectXResource.m_UsageState;

		D3D12_RESOURCE_STATES NewState = ResourceStatesToDirectXResourceStates(State);

		if (m_Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			PX_CORE_ASSERT((OldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == OldState, "resource state is not valid!");
			PX_CORE_ASSERT((NewState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == NewState, "resource state is not valid!");
		}

		if (OldState != NewState)
		{
			PX_CORE_ASSERT(m_NumBarriersToFlush < 16, "exceeded arbitrary limit on buffered barriers!");
			//add a new barrier
			D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

			BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			BarrierDesc.Transition.pResource = DirectXResource.GetResource();
			BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			BarrierDesc.Transition.StateBefore = OldState;
			BarrierDesc.Transition.StateAfter = NewState;

			//check to see if we already started the transition
			if (NewState == DirectXResource.m_TransitioningState)
			{
				BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
				DirectXResource.m_TransitioningState = (D3D12_RESOURCE_STATES)-1;
			}
			else
				BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			DirectXResource.m_UsageState = NewState;
		}//oldstate == newstate && newstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		else if (NewState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			InsertUAVBarrier(Resource, FlushImmediate);
		}

		if (FlushImmediate || m_NumBarriersToFlush == 16)
			FlushResourceBarriers();
	}

	void DirectXContext::InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate)
	{
		PX_CORE_ASSERT(m_NumBarriersToFlush < 16, "exceeded arbitrary limit on buffered barriers");
		D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		BarrierDesc.UAV.pResource = static_cast<DirectXGpuResource&>(Resource).GetResource();

		if (FlushImmediate)
			FlushResourceBarriers();
	}

	void DirectXContext::InitializeTexture(GpuResource& Dest, uint32_t NumSubresources, D3D12_SUBRESOURCE_DATA SubData[])
	{
		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		//returns the required size of a buffer to be used for data upload
		uint64_t uploadBufferSize = GetRequiredIntermediateSize(DestResource.GetResource(), 0, NumSubresources);

		//copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
		DynAlloc mem = ReserveUploadMemory(uploadBufferSize);
		UpdateSubresources(m_pCommandList.Get(), DestResource.GetResource(), mem.Buffer.GetResource(), 0, 0, NumSubresources, SubData);
		TransitionResource(Dest, ResourceStates::GenericRead);
		//execute the command list and wait for it to finish so we can release the upload buffer
		//finish can release upload buffer
		Finish(true);
	}

	void DirectXContext::InitializeBuffer(GpuResource& Dest, const void* BufferData, size_t NumBytes, size_t DestOffset /*= 0*/)
	{
		DirectXGpuResource& DestBuffer = static_cast<DirectXGpuBuffer&>(Dest);

		DynAlloc mem = ReserveUploadMemory(NumBytes);
		memcpy(mem.DataPtr, BufferData, Math::DivideByMultiple(NumBytes, 16) * 16);
		
		//copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
		TransitionResource(DestBuffer, ResourceStates::CopyDest, true);
		m_pCommandList->CopyBufferRegion(DestBuffer.m_pResource.Get(), DestOffset, mem.Buffer.GetResource(), 0, NumBytes);
		TransitionResource(DestBuffer, ResourceStates::GenericRead, true);

		//execute the command list and wait for it to finish so we can release the upload buffer
		Finish(true);
	}

	void DirectXContext::InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src)
	{
		TransitionResource(Dest, ResourceStates::CopyDest);
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Src);

		const D3D12_RESOURCE_DESC& DestDesc = DestResource.GetResource()->GetDesc();
		const D3D12_RESOURCE_DESC& SrcDesc = SrcResource.GetResource()->GetDesc();

		PX_CORE_ASSERT(SliceIndex < DestDesc.DepthOrArraySize&&
			SrcDesc.DepthOrArraySize == 1 &&
			DestDesc.Width == SrcDesc.Width &&
			DestDesc.Height == SrcDesc.Height &&
			DestDesc.MipLevels <= SrcDesc.MipLevels, "slice index is error!"
		);

		uint32_t SubResourceIndex = SliceIndex * DestDesc.MipLevels;

		for (uint32_t i = 0; i < DestDesc.MipLevels; ++i)
		{
			D3D12_TEXTURE_COPY_LOCATION destCopyLocation = {
				DestResource.GetResource(),
				D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				SubResourceIndex + i
			};

			D3D12_TEXTURE_COPY_LOCATION srcCopyLocation =
			{
				SrcResource.GetResource(),
				D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				i
			};

			m_pCommandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);
		};

		TransitionResource(Dest, ResourceStates::GenericRead);

		Finish(true);
	}

	void DirectXContext::WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* BufferData, size_t NumBytes)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is not aligned!");

		DynAlloc TempSpace = m_CpuLinearAllocator.Allocate(NumBytes, 512);
		memcpy(TempSpace.DataPtr, BufferData, Math::DivideByMultiple(NumBytes, 16) * 16);
		CopyBufferRegion(Dest, DestOffset, TempSpace.Buffer, TempSpace.Offset, NumBytes);
	}

	DynAlloc DirectXContext::ReserveUploadMemory(size_t SizeInBytes)
	{
		return m_CpuLinearAllocator.Allocate(SizeInBytes, 256);
	}

	void DirectXContext::SetDescriptorHeap(DescriptorHeapType Type, Ref<DescriptorHeap> HeapPtr)
	{
		Ref<DirectXDescriptorHeap> Heap = std::static_pointer_cast<DirectXDescriptorHeap>(HeapPtr);
		if (m_CurrentDescriptorHeaps[DescriptorHeapTypeToDirectXDescriptorHeapType(Type)] != Heap->GetComPtrHeap())
		{
			m_CurrentDescriptorHeaps[DescriptorHeapTypeToDirectXDescriptorHeapType(Type)] = Heap->GetComPtrHeap();
			BindDescriptorHeaps();
		}
	}

	void DirectXContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, ID3D12DescriptorHeap* HeapPtr)
	{
		if (m_CurrentDescriptorHeaps[Type].Get() != HeapPtr)
		{
			m_CurrentDescriptorHeaps[Type] = HeapPtr;
			BindDescriptorHeaps();
		}
	}

	void DirectXContext::SetDescriptorHeaps(uint32_t HeapCount, D3D12_DESCRIPTOR_HEAP_TYPE Type[], Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtrs[])
	{
		bool AnyChanged = false;
		for (uint32_t i = 0; i < HeapCount; ++i)
		{
			if (m_CurrentDescriptorHeaps[Type[i]] != HeapPtrs[i])
			{
				m_CurrentDescriptorHeaps[Type[i]] = HeapPtrs[i];
				AnyChanged = true;
			}
		}

		if (AnyChanged)
			BindDescriptorHeaps();
	}

	void DirectXContext::SetPipelineState(const PSO& pso)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState = static_cast<const GraphicsPSO&>(pso).GetPipelineStateObject();

		if (PipelineState == m_CurrPipelineState || PipelineState == nullptr)
			return;

		m_pCommandList->SetPipelineState(PipelineState.Get());
		m_CurrPipelineState = PipelineState;
	}

	GraphicsContext& DirectXContext::GetGraphicsContext()
	{
		PX_CORE_ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_COMPUTE, "cannot convert async compute context to graphics!");
		return static_cast<GraphicsContext&>(*this);
	}

	void DirectXContext::ClearColor(GpuResource& Target, PixelRect* Rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect /*= nullptr*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::ClearDepth(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::ClearStencil(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::ClearDepthAndStencil(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDepthStencilTarget(Ref<DescriptorCpuHandle> DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetRootSignature(const RootSignature& RootSig)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetViewport(const ViewPort& vp)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetViewport(float x, float y, float w, float h, float minDepth /*= 0.0f*/, float maxDepth /*= 1.0f*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetScissor(const PixelRect& rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetViewportAndScissor(const ViewPort& vp, const PixelRect& rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetStencilRef(uint32_t StencilRef)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetBlendFactor(glm::vec4 BlendFactor)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetPrimitiveTopology(PrimitiveTopology Topology)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstants(uint32_t RootIndex, uint32_t x)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetIndexBuffer(const Ref<IBV> IBView)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDynamicIB(size_t IndexCount, const uint16_t* IBData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::Draw(uint32_t VertexCount, uint32_t VertexStartOffset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation /*= 0*/, int32_t BaseVertexLocation /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation /*= 0*/, uint32_t StartInstanceLocation /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	CommandListType DirectXContext::GetType()
	{
		//throw std::logic_error("The method or operation is not implemented.");
		return DirectXCmdListTypeToCmdListType(m_Type);
	}

	void DirectXContext::Dispatch(size_t GroupCountX /*= 1*/, size_t GroupCountY /*= 1*/, size_t GroupCountZ /*= 1*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::Dispatch1D(size_t ThreadCountX, size_t GroupSizeX /*= 64*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::Dispatch2D(size_t ThreadCountX, size_t ThreadCountY, size_t GroupSizeX /*= 8*/, size_t GroupSizeY /*= 8*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXContext::SetType(CommandListType type)
	{
		m_Type = CmdListTypeToDirectXCmdListType(type);
	}

	void DirectXContext::Reset()
	{
		m_pCurrentAllocator = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->GetQueue(m_Type).RequesetAlloactor();
		m_pCommandList->Reset(m_pCurrentAllocator.Get(), nullptr);

		m_CurrGraphicsRootSignature = nullptr;
		m_CurrComputeRootSingature = nullptr;
		m_CurrPipelineState = nullptr;
		m_NumBarriersToFlush = 0;

		BindDescriptorHeaps();
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DirectXContext::GetCommandList()
	{
		return m_pCommandList;
	}

	void* DirectXContext::GetNativeCommandList()
	{
		return m_pCommandList.Get();
	}

	//-------DirectX Context------
}