#include "pxpch.h"
#include "DirectXDynamicDescriptorHeap.h"

#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/Command/CommandQueue.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/Context/DirectXContext.h"

namespace Pixel {

	//------Descriptor Handle Cache------
	DescriptorHandleCache::DescriptorHandleCache()
	{
		ClearCache();
	}

	void DescriptorHandleCache::ClearCache()
	{
		m_RootDescriptorTablesBitMap = 0;
		m_StaleRootParamsBitMap = 0;
		m_MaxCachedDescriptors = 0;
	}

	uint32_t DescriptorHandleCache::ComputeStagedSize()
	{
		//sum the maximum assigned offsets of stale descriptor tables to determine total needed space
		uint32_t NeededSpace = 0;
		uint32_t RootIndex;
		uint32_t StaleParams = m_StaleRootParamsBitMap;

		//from the least significant bit to most significant bit to search, to find a bit one
		while (_BitScanForward((unsigned long*)(&RootIndex), StaleParams))
		{
			StaleParams ^= (1 << RootIndex);

			uint32_t MaxSetHandle;
			PX_CORE_ASSERT(_BitScanReverse((unsigned long*)(&MaxSetHandle), m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap) == true,
			"root entry marked as stale but has no stale descriptors");

			NeededSpace += MaxSetHandle + 1;
		}
		return NeededSpace;
	}

	void DescriptorHandleCache::CopyAndBindStaleTables(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t DescriptorSize,
	DescriptorHandle DestHandleStart, ID3D12GraphicsCommandList* CmdList, 
	void(STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
	{
		uint32_t StaleParamCount = 0;

		uint32_t TableSize[DescriptorHandleCache::kMaxNumDescriptorTables];
		uint32_t RootIndices[DescriptorHandleCache::kMaxNumDescriptorTables];
		uint32_t NeededSpace = 0;
		uint32_t RootIndex;

		//sum the maxmum assigned offsets of stale descriptor tables to determine total needed space
		uint32_t StaleParams = m_StaleRootParamsBitMap;

		while (_BitScanForward((unsigned long*)&RootIndex, StaleParams))
		{
			RootIndices[StaleParamCount] = RootIndex;
			StaleParams ^= (1 << RootIndex);

			uint32_t MaxSetHandle;
			PX_CORE_ASSERT(_BitScanReverse((unsigned long*)&MaxSetHandle, m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap) == true,
				"root entry marked as stale but has no stale descriptors");

			NeededSpace += MaxSetHandle + 1;
			TableSize[StaleParamCount] = MaxSetHandle + 1;

			++StaleParamCount;
		}

		PX_CORE_ASSERT(StaleParamCount <= DescriptorHandleCache::kMaxNumDescriptorTables,
			"we're only equipped to handle so many descriptor tables");

		m_StaleRootParamsBitMap = 0;

		static const uint32_t kMaxDescriptorsPerCopy = 16;
		uint32_t NumDestDescriptorRanges = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[kMaxDescriptorsPerCopy];
		uint32_t pDestDescriptorRangeSizes[kMaxDescriptorsPerCopy];

		uint32_t NumSrcDescriptorRanges = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE pSrcDescriptorRangeStarts[kMaxDescriptorsPerCopy];
		uint32_t pSrcDescriptorRangeSizes[kMaxDescriptorsPerCopy];

		for (uint32_t i = 0; i < StaleParamCount; ++i)
		{
			RootIndex = RootIndices[i];
			//SetGraphicsRootDescriptorTable(RootParamter, Heap's Handle)
			Ref<DirectXDescriptorGpuHandle> gpuHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(DestHandleStart.GetGpuHandle());
			(CmdList->*SetFunc)(RootIndex, gpuHandle->GetGpuHandle());

			DescriptorTableCache& RootDescTable = m_RootDescriptorTable[RootIndex];

			D3D12_CPU_DESCRIPTOR_HANDLE* SrcHandles = RootDescTable.TableStart;
			uint64_t SetHandles = (uint64_t)RootDescTable.AssignedHandlesBitMap;
			Ref<DirectXDescriptorCpuHandle> cpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DestHandleStart.GetCpuHandle());
			D3D12_CPU_DESCRIPTOR_HANDLE CurDest = cpuHandle->GetCpuHandle();
			DestHandleStart += TableSize[i] * DescriptorSize;

			uint32_t SkipCount;
			while (_BitScanForward64((unsigned long*)&SkipCount, SetHandles))
			{
				//skip over unset descriptor handles
				SetHandles >>= SkipCount;
				SrcHandles += SkipCount;
				CurDest.ptr += SkipCount * DescriptorSize;

				unsigned long DescriptorCount;
				_BitScanForward64(&DescriptorCount, ~SetHandles);
				SetHandles >>= DescriptorCount;

				//if we run out of temp room, copy what we've got so for
				if (NumSrcDescriptorRanges + DescriptorCount > kMaxDescriptorsPerCopy)
				{
					std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CopyDescriptors(
						NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
						NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
						Type
					);

					NumSrcDescriptorRanges = 0;
					NumDestDescriptorRanges = 0;
				}

				//setup destination range
				pDestDescriptorRangeStarts[NumDestDescriptorRanges] = CurDest;
				pDestDescriptorRangeSizes[NumDestDescriptorRanges] = DescriptorCount;
				++NumDestDescriptorRanges;

				//setup source ranges(one descriptor each because we don't assume they are contignous)
				for (uint32_t j = 0; j < DescriptorCount; ++j)
				{
					pSrcDescriptorRangeStarts[NumDestDescriptorRanges] = SrcHandles[j];
					pSrcDescriptorRangeSizes[NumDestDescriptorRanges] = 1;
					++NumSrcDescriptorRanges;
				}

				//move the destination pointer forward by the number of descriptors we will copy
				SrcHandles += DescriptorCount;
				CurDest.ptr += DescriptorCount * DescriptorSize;
			}
		}

		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CopyDescriptors(
			NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
			NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
			Type
		);
	}

	void DescriptorHandleCache::UnBindAllValid()
	{
		m_StaleRootParamsBitMap = 0;

		unsigned long TableParams = m_RootDescriptorTablesBitMap;
		unsigned long RootIndex;

		while (_BitScanForward(&RootIndex, TableParams))
		{
			//get rid of one bit
			TableParams ^= (1 << RootIndex);
			if (m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap != 0)
				m_StaleRootParamsBitMap |= (1 << RootIndex);
		}
	}

	void DescriptorHandleCache::StageDescriptorHandles(uint32_t RootIndex, uint32_t Offset, uint32_t NumHandles, 
	const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
	{
		PX_CORE_ASSERT(((1 << RootIndex) & m_RootDescriptorTablesBitMap) != 0, "root parameter is not a cbv_srv_uav descriptor table!");

		//table size is computed from the parse root signature
		PX_CORE_ASSERT(Offset + NumHandles <= m_RootDescriptorTable[RootIndex].TableSize, "out of the handle range");

		DescriptorTableCache& TableCache = m_RootDescriptorTable[RootIndex];
		//table start reference's descriptor handle cache's m_HandleCache array
		D3D12_CPU_DESCRIPTOR_HANDLE* CopyDest = TableCache.TableStart + Offset;
		for (uint32_t i = 0; i < NumHandles; ++i)
			CopyDest[i] = Handles[i];

		TableCache.AssignedHandlesBitMap |= ((1 << NumHandles) - 1) << Offset;
		m_StaleRootParamsBitMap |= (1 << RootIndex);
	}

	void DescriptorHandleCache::ParseRootSignature(D3D12_DESCRIPTOR_HEAP_TYPE Type, const RootSignature& RootSig)
	{
		uint32_t CurrentOffset = 0;

		DirectXRootSignature& pRootSig = static_cast<DirectXRootSignature&>(const_cast<RootSignature&>(RootSig));

		PX_CORE_ASSERT(pRootSig.m_NumParameters <= 16, "we need to support something greater!");

		m_StaleRootParamsBitMap = 0;
		m_RootDescriptorTablesBitMap = (Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ?
			pRootSig.m_SamplerTableBitMap : pRootSig.m_DescriptorTableBitMap);

		unsigned long TableParams = m_RootDescriptorTablesBitMap;
		unsigned long RootIndex;
		while (_BitScanForward(&RootIndex, TableParams))
		{
			TableParams ^= (1 << RootIndex);

			uint32_t TableSize = pRootSig.m_DescriptorTableSize[RootIndex];
			PX_CORE_ASSERT(TableSize > 0, "table size is not greater than 0!");

			DescriptorTableCache& RootDescriptorTable = m_RootDescriptorTable[RootIndex];
			RootDescriptorTable.AssignedHandlesBitMap = 0;
			RootDescriptorTable.TableStart = m_HandleCache + CurrentOffset;
			RootDescriptorTable.TableSize = TableSize;

			CurrentOffset += TableSize;
		}

		m_MaxCachedDescriptors = CurrentOffset;

		PX_CORE_ASSERT(m_MaxCachedDescriptors <= kMaxNumDescriptors, "exceeded user-supplied maxmium cache size!");
	}

	//------Descriptor Handle Cache------
	const uint32_t DirectXDynamicDescriptorHeap::kNumDescriptorPerHeap;
	std::mutex DirectXDynamicDescriptorHeap::sm_Mutex;
	std::queue<std::pair<uint64_t, ID3D12DescriptorHeap*>> DirectXDynamicDescriptorHeap::sm_RetiredDescriptorHeaps[2];
	std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> DirectXDynamicDescriptorHeap::sm_DescriptorHeapPool[2];
	std::queue<ID3D12DescriptorHeap*> DirectXDynamicDescriptorHeap::sm_AvailableDescriptorHeaps[2];

	DirectXDynamicDescriptorHeap::DirectXDynamicDescriptorHeap(Context& OwningContext, DescriptorHeapType HeapType)
		:m_OwingContext(static_cast<DirectXContext&>(OwningContext)), m_DescriptorType(DescriptorHeapTypeToDirectXDescriptorHeapType(HeapType))
	{
		m_CurrentHeapPtr = nullptr;
		m_CurrentOffset = 0;
		m_DescriptorSize = std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->GetDescriptorHandleIncrementSize(m_DescriptorType);
	}

	DirectXDynamicDescriptorHeap::~DirectXDynamicDescriptorHeap()
	{

	}

	void DirectXDynamicDescriptorHeap::DestroyAll()
	{
		sm_DescriptorHeapPool[0].clear();
		sm_DescriptorHeapPool[1].clear();
	}

	void DirectXDynamicDescriptorHeap::CleanupUsedHeaps(uint64_t fenceValue)
	{
		RetireCurrentHeap();
		RetireUsedHeaps(fenceValue);
		m_GraphicsHandleCache.ClearCache();
		m_ComputeHandleCache.ClearCache();
	}

	void DirectXDynamicDescriptorHeap::SetGraphicsDescriptorHandles(uint32_t RootIndex, uint32_t Offset, 
	uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[])
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		for (uint32_t i = 0; i < NumHandles; ++i)
		{
			Ref<DirectXDescriptorCpuHandle> cpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Handles[i]);
			handles.push_back(cpuHandle->GetCpuHandle());
		}

		m_GraphicsHandleCache.StageDescriptorHandles(RootIndex, Offset, NumHandles, &handles[0]);
	}

	void DirectXDynamicDescriptorHeap::SetComputeDescriptorHandles(uint32_t RootIndex, uint32_t Offset, 
	uint32_t NumHandles, const Ref<DescriptorCpuHandle> Handles[])
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

		for (uint32_t i = 0; i < NumHandles; ++i)
		{
			Ref<DirectXDescriptorCpuHandle> cpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Handles[i]);
			handles.push_back(cpuHandle->GetCpuHandle());
		}

		m_ComputeHandleCache.StageDescriptorHandles(RootIndex, Offset, NumHandles, &handles[0]);
	}

	Ref<DescriptorGpuHandle> DirectXDynamicDescriptorHeap::UploadDirect(Ref<DescriptorCpuHandle> Handles)
	{
		if (!HasSpace(1))
		{
			RetireCurrentHeap();
			UnBindAllValid();
		}

		m_OwingContext.SetDescriptorHeap(m_DescriptorType, GetHeapPointer());

		DescriptorHandle DestHandle = m_FirstDescriptor + m_CurrentOffset * m_DescriptorSize;
		m_CurrentOffset += 1;

		Ref<DirectXDescriptorCpuHandle> cpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DestHandle.GetCpuHandle());

		D3D12_CPU_DESCRIPTOR_HANDLE tempHandle;
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CopyDescriptorsSimple(1, tempHandle, std::static_pointer_cast<DirectXDescriptorCpuHandle>(Handles)->GetCpuHandle(), m_DescriptorType);
		cpuHandle->SetCpuHandle(tempHandle);
		
		return DestHandle.GetGpuHandle();
	}

	void DirectXDynamicDescriptorHeap::ParseGraphicsRootSignature(const RootSignature& RootSig)
	{
		m_GraphicsHandleCache.ParseRootSignature(m_DescriptorType, RootSig);
	}

	void DirectXDynamicDescriptorHeap::ParseComputeRootSignature(const RootSignature& RootSig)
	{
		m_ComputeHandleCache.ParseRootSignature(m_DescriptorType, RootSig);
	}

	void DirectXDynamicDescriptorHeap::CommitGraphicsRootDescriptorTables(ID3D12GraphicsCommandList* CmdList)
	{
		if (m_GraphicsHandleCache.m_StaleRootParamsBitMap != 0)
			CopyAndBindStagedTables(m_GraphicsHandleCache, CmdList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
	}

	void DirectXDynamicDescriptorHeap::CommitComputeRootDescriptorTables(ID3D12GraphicsCommandList* CmdList)
	{
		if (m_ComputeHandleCache.m_StaleRootParamsBitMap != 0)
			CopyAndBindStagedTables(m_ComputeHandleCache, CmdList, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
	}

	ID3D12DescriptorHeap* DirectXDynamicDescriptorHeap::RequestDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
	{
		std::lock_guard<std::mutex> LockGuard(sm_Mutex);

		uint32_t idx = HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0;

		while (!sm_RetiredDescriptorHeaps[idx].empty() && std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetCommandListManager()->IsFenceComplete(sm_RetiredDescriptorHeaps[idx].front().first))
		{
			sm_AvailableDescriptorHeaps[idx].push(sm_RetiredDescriptorHeaps[idx].front().second);
			sm_RetiredDescriptorHeaps[idx].pop();
		}

		if (!sm_AvailableDescriptorHeaps[idx].empty())
		{
			ID3D12DescriptorHeap* HeapPtr = sm_AvailableDescriptorHeaps[idx].front();
			sm_AvailableDescriptorHeaps[idx].pop();
			return HeapPtr;
		}
		else
		{
			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
			HeapDesc.Type = HeapType;
			HeapDesc.NumDescriptors = kNumDescriptorPerHeap;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HeapDesc.NodeMask = 1;
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr;
			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(HeapPtr.GetAddressOf())) >= 0,
			"create descriptor heap error!");
			sm_DescriptorHeapPool[idx].emplace_back(HeapPtr);
			return HeapPtr.Get();
		}
	}

	void DirectXDynamicDescriptorHeap::DiscardDescriptorHeaps(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint64_t FenceValue, 
	const std::vector<ID3D12DescriptorHeap*>& UsedHeaps)
	{
		uint32_t idx = HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0;
		std::lock_guard<std::mutex> LockGuard(sm_Mutex);

		for (auto iter = UsedHeaps.begin(); iter != UsedHeaps.end(); ++iter)
		{
			sm_RetiredDescriptorHeaps[idx].push(std::make_pair(FenceValue, *iter));
		}
	}

	bool DirectXDynamicDescriptorHeap::HasSpace(uint32_t Count)
	{
		return (m_CurrentHeapPtr != nullptr && m_CurrentOffset + Count <= kNumDescriptorPerHeap);
	}

	void DirectXDynamicDescriptorHeap::RetireCurrentHeap()
	{
		if (m_CurrentOffset == 0)
		{
			PX_CORE_ASSERT(m_CurrentHeapPtr == nullptr, "current heap ptr is not nullptr");
			return;
		}

		PX_CORE_ASSERT(m_CurrentHeapPtr != nullptr, "current heap ptr is nullptr");
		m_RetriedHeaps.push_back(m_CurrentHeapPtr);
		m_CurrentHeapPtr = nullptr;
		m_CurrentOffset = 0;
	}

	void DirectXDynamicDescriptorHeap::RetireUsedHeaps(uint64_t fanceValue)
	{
		DiscardDescriptorHeaps(m_DescriptorType, fanceValue, m_RetriedHeaps);
		m_RetriedHeaps.clear();
	}

	ID3D12DescriptorHeap* DirectXDynamicDescriptorHeap::GetHeapPointer()
	{
		if (m_CurrentHeapPtr == nullptr)
		{
			PX_CORE_ASSERT(m_CurrentOffset == 0, "current offset must be 0!");
			m_CurrentHeapPtr = RequestDescriptorHeap(m_DescriptorType);

			Ref<DirectXDescriptorCpuHandle> cpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
			cpuHandle->SetCpuHandle(m_CurrentHeapPtr->GetCPUDescriptorHandleForHeapStart());
			Ref<DirectXDescriptorGpuHandle> gpuHandle = std::make_shared<DirectXDescriptorGpuHandle>();
			gpuHandle->SetGpuHandle(m_CurrentHeapPtr->GetGPUDescriptorHandleForHeapStart());

			m_FirstDescriptor = DescriptorHandle(
				std::static_pointer_cast<DescriptorCpuHandle>(cpuHandle),
				std::static_pointer_cast<DescriptorGpuHandle>(gpuHandle)
			);		
		}

		return m_CurrentHeapPtr;
	}

	DescriptorHandle DirectXDynamicDescriptorHeap::Allocate(uint32_t Count)
	{
		DescriptorHandle ret = m_FirstDescriptor + m_CurrentOffset * m_DescriptorSize;
		m_CurrentOffset += Count;
		return ret;
	}

	void DirectXDynamicDescriptorHeap::CopyAndBindStagedTables(DescriptorHandleCache& HandleCache, ID3D12GraphicsCommandList* CmdList,
	void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
	{
		uint32_t NeededSize = HandleCache.ComputeStagedSize();
		if (!HasSpace(NeededSize))
		{
			RetireCurrentHeap();
			UnBindAllValid();
			NeededSize = HandleCache.ComputeStagedSize();
		}

		//this can trigger the creation of a new hap
		m_OwingContext.SetDescriptorHeap(m_DescriptorType, GetHeapPointer());
		HandleCache.CopyAndBindStaleTables(m_DescriptorType, m_DescriptorSize, Allocate(NeededSize), CmdList, SetFunc);
	}

	void DirectXDynamicDescriptorHeap::UnBindAllValid()
	{
		m_GraphicsHandleCache.UnBindAllValid();
		m_ComputeHandleCache.UnBindAllValid();
	}

}