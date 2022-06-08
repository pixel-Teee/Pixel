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
#include "Pixel/Math/Math.h"

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
	DirectXContext::DirectXContext(CommandListType Type, Ref<ContextManager> pContextManager)
		:m_Type(DirectXContextManager::CommandListTypeToDirectXCommandListType(Type)),
		m_CpuLinearAllocator(kCpuWritable),
		m_GpuLinearAllocator(kGpuExclusive),
		m_DynamicViewDescriptorHeap(*this, DescriptorHeapType::CBV_UAV_SRV),
		m_DynamicSamplerDescriptorHeap(*this, DescriptorHeapType::SAMPLER),
		m_OwingContextManager(pContextManager)
	{
		
	}

	DirectXContext::~DirectXContext()
	{
	}

	void DirectXContext::Initialize()
	{
		CommandListManager::Get()->CreateNewCommandList(m_Type, m_pCommandList, m_pCurrentAllocator);
	}

	void DirectXContext::SwapBuffers()
	{
	
	}

	uint64_t DirectXContext::Flush(bool WaitForCompletion)
	{
		FlushResourceBarriers();

		uint64_t FenceValue = CommandListManager::Get()->GetQueue(m_Type).ExecuteCommandList(m_pCommandList);

		if (WaitForCompletion)
			CommandListManager::Get()->WaitForFence(FenceValue);

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

		CommandQueue& Queue = CommandListManager::Get()->GetQueue(m_Type);

		uint64_t FenceValue = Queue.ExecuteCommandList(m_pCommandList);
		Queue.DiscardAlloactor(FenceValue, m_pCurrentAllocator);
		//release and call the instance's release function
		m_pCurrentAllocator->Reset();

		if (WaitForCompletion)
			CommandListManager::Get()->WaitForFence(FenceValue);

		//g_ContextManager.FreeContext(shared_from_this());

		m_OwingContextManager->FreeContext(shared_from_this());

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
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
		FlushResourceBarriers();

		DirectXGpuResource& DestResource = static_cast<DirectXGpuResource&>(Dest);
		DirectXGpuResource& SrcResource = static_cast<DirectXGpuResource&>(Src);

		//------Copy Resource------
		m_pCommandList->CopyResource(DestResource.GetResource(), SrcResource.GetResource());
		//------Copy Resource------
	}

	void DirectXContext::CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes)
	{
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
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
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResource(Source, D3D12_RESOURCE_STATE_COPY_SOURCE);
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

	void DirectXContext::TransitionResource(GpuResource& Resource, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
	{
		DirectXGpuResource& DirectXResource = static_cast<DirectXGpuResource&>(Resource);
		D3D12_RESOURCE_STATES OldState = DirectXResource.m_UsageState;

		if (m_Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			PX_CORE_ASSERT((OldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == OldState, "resource state is not valid!");
			PX_CORE_ASSERT((OldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == NewState, "resource state is not valid!");
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
		TransitionResource(Dest, D3D12_RESOURCE_STATE_GENERIC_READ);
		//execute the command list and wait for it to finish so we can release the upload buffer
		//finish can release upload buffer
		Finish(true);
	}

	void DirectXContext::InitializeBuffer(GpuBuffer& Dest, const void* BufferData, size_t NumBytes, size_t DestOffset /*= 0*/)
	{
		DirectXGpuBuffer& DestBuffer = static_cast<DirectXGpuBuffer&>(Dest);

		DynAlloc mem = ReserveUploadMemory(NumBytes);
		memcpy(mem.DataPtr, BufferData, Math::DivideByMultiple(NumBytes, 16) * 16);
		
		//copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
		TransitionResource(DestBuffer.m_GpuResource, D3D12_RESOURCE_STATE_COPY_DEST, true);
		m_pCommandList->CopyBufferRegion(DestBuffer.m_GpuResource.m_pResource.Get(), DestOffset, mem.Buffer.GetResource(), 0, NumBytes);
		TransitionResource(DestBuffer.m_GpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, true);

		//execute the command list and wait for it to finish so we can release the upload buffer
		Finish(true);
	}

	void DirectXContext::InitializeTextureArraySlice(GpuResource& Dest, uint32_t SliceIndex, GpuResource& Src)
	{
		TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
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

		TransitionResource(Dest, D3D12_RESOURCE_STATE_GENERIC_READ);

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
		return m_CpuLinearAllocator.Allocate(SizeInBytes);
	}

	void DirectXContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr)
	{
		if (m_CurrentDescriptorHeaps[Type] != HeapPtr)
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

	void DirectXContext::SetPipelineState(const DirectXPSO& pso)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState = pso.GetPipelineStateObject();

		if (PipelineState == m_CurrPipelineState)
			return;

		m_pCommandList->SetPipelineState(PipelineState.Get());
		m_CurrPipelineState = PipelineState;
	}

	GraphicsContext& DirectXContext::GetGraphicsContext()
	{
		PX_CORE_ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_COMPUTE, "cannot convert async compute context to graphics!");
		return static_cast<GraphicsContext&>(*this);
	}

	void DirectXContext::Reset()
	{
		m_pCurrentAllocator = CommandListManager::Get()->GetQueue(m_Type).RequesetAlloactor();
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

	//-------DirectX Context------
}