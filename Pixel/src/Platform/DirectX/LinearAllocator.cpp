#include "pxpch.h"
#include "LinearAllocator.h"

#include "DirectXDevice.h"
#include "CommandQueue.h"
#include "Pixel/Math/Math.h"

namespace Pixel {

	LinearAllocationPage::LinearAllocationPage(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Usage)
		:DirectXGpuResource()
	{
		m_pResource.Attach(pResource);
		m_UsageState = Usage;
		m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();
		m_pResource->Map(0, nullptr, &m_CpuVirtualAddress);
	}

	LinearAllocationPage::~LinearAllocationPage()
	{
		UnMap();
	}

	void LinearAllocationPage::Map()
	{
		if (m_CpuVirtualAddress == nullptr)
		{
			//0:subresource index
			//nullptr:entire subresource
			m_pResource->Map(0, nullptr, &m_CpuVirtualAddress);
		}
	}

	void LinearAllocationPage::UnMap()
	{
		if (m_CpuVirtualAddress != nullptr)
		{
			m_pResource->Unmap(0, nullptr);
			m_CpuVirtualAddress = nullptr;
		}
	}

	//------Linear Allocator Page Manager------
	LinearAllocatorType LinearAllocatorPageManager::sm_AutoType = kGpuExclusive;

	LinearAllocatorPageManager::LinearAllocatorPageManager()
	{
		m_AllocationType = sm_AutoType;
		sm_AutoType = (LinearAllocatorType)(sm_AutoType + 1);
		PX_CORE_ASSERT(sm_AutoType <= kNumAllocatorTypes, "allocator page type exceeds range!");
	}

	LinearAllocationPage* LinearAllocatorPageManager::RequestPage()
	{
		std::lock_guard<std::mutex> LockGuard(m_Mutex);

		//from the retried pages extract have completed's pages
		while (!m_RetriedPages.empty() && CommandListManager::Get()->IsFenceComplete(m_RetriedPages.front().first))
		{
			m_AvailablePages.push(m_RetriedPages.front().second);
			m_RetriedPages.pop();
		}

		LinearAllocationPage* PagePtr = nullptr;
		if (!m_AvailablePages.empty())
		{
			PagePtr = m_AvailablePages.front();
			m_AvailablePages.pop();
		}
		else
		{
			PagePtr = CreateNewPage();
			m_PagePool.emplace_back(PagePtr);
		}

		return PagePtr;
	}

	LinearAllocationPage* LinearAllocatorPageManager::CreateNewPage(size_t PageSize)
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC ResourceDesc;
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_RESOURCE_STATES DefaultUsage;
		//default heap
		if (m_AllocationType == kGpuExclusive)
		{
			HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			ResourceDesc.Width = PageSize == 0 ? kGpuAllocatorPageSize : PageSize;
			ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			DefaultUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		else
		{
			//upload heap
			HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			ResourceDesc.Width = PageSize == 0 ? kCpuAllocatorPageSize : PageSize;
			ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			DefaultUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		ID3D12Resource* pBuffer;
		PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE,
			&ResourceDesc, DefaultUsage, nullptr, IID_PPV_ARGS(&pBuffer)) >= 0, "create resource error!");

		pBuffer->SetName(L"LinearAllocator Page");

		return new LinearAllocationPage(pBuffer, DefaultUsage);
	}

	void LinearAllocatorPageManager::DiscardPages(uint64_t FenceValue, const std::vector<LinearAllocationPage*>& UsedPages)
	{
		std::lock_guard<std::mutex> LockGuard(m_Mutex);

		for (auto iter = UsedPages.begin(); iter != UsedPages.end(); ++iter)
			m_RetriedPages.push(std::make_pair(FenceValue, *iter));
	}

	void LinearAllocatorPageManager::FreeLargePages(uint64_t FenceValue, const std::vector<LinearAllocationPage*> LargePages)
	{
		std::lock_guard<std::mutex> LockGuard(m_Mutex);

		while (!m_DeletionQueue.empty() && CommandListManager::Get()->IsFenceComplete(m_DeletionQueue.front().first))
		{
			delete m_DeletionQueue.front().second;
			m_DeletionQueue.pop();
		}

		for (auto iter = LargePages.begin(); iter != LargePages.end(); ++iter)
		{
			(*iter)->UnMap();
			m_DeletionQueue.push(std::make_pair(FenceValue, *iter));
		}
	}

	void LinearAllocatorPageManager::Destroy()
	{
		m_PagePool.clear();
	}

	
	//------Linear Allocator Page Manager------

	//------Linear Allocator------
	LinearAllocatorPageManager LinearAllocator::sm_PageManager[2];

	LinearAllocator::LinearAllocator(LinearAllocatorType Type)
	:m_AllocationType(Type), m_PageSize(0), m_CurrOffset(0), m_CurrPage(nullptr)
	{
		PX_CORE_ASSERT(Type > kInvalidAllocator && Type < kNumAllocatorTypes, "buffer type is error!");

		m_PageSize = (Type == kGpuExclusive ? kGpuAllocatorPageSize : kCpuAllocatorPageSize);
	}

	Pixel::DynAlloc LinearAllocator::Allocate(size_t SizeInBytes, size_t Alignment)
	{
		//2^n - 1
		const size_t AlignmentMask = Alignment - 1;

		PX_CORE_ASSERT((AlignmentMask & Alignment) == 0, "alignment is not 2's powner!");

		//align the allocation
		const size_t AlignedSize = Math::AlignUpWithMask(SizeInBytes, AlignmentMask);

		if (AlignedSize > m_PageSize)
			return AllocateLargePage(AlignedSize);


		//------AlignedSize < m_PageSize------
		m_CurrOffset = Math::AlignUp(m_CurrOffset, Alignment);

		if (m_CurrOffset + AlignedSize > m_PageSize)
		{
			PX_CORE_ASSERT(m_CurrPage != nullptr, "current page is nullptr!");
			m_RetiredPages.push_back(m_CurrPage);
			m_CurrPage = nullptr;
		}

		if (m_CurrPage == nullptr)
		{
			m_CurrPage = sm_PageManager[m_AllocationType].RequestPage();
			m_CurrOffset = 0;
		}

		DynAlloc ret(*m_CurrPage, m_CurrOffset, AlignedSize);
		ret.DataPtr = (uint8_t*)m_CurrPage->m_CpuVirtualAddress + m_CurrOffset;
		ret.GpuAddress = m_CurrPage->m_GpuVirtualAddress + m_CurrOffset;

		m_CurrOffset += AlignedSize;
		//------AlignedSize < m_PageSize------
		return ret;
	}

	void LinearAllocator::CleanupUsedPages(uint64_t FenceId)
	{
		if (m_CurrPage == nullptr) return;

		m_RetiredPages.push_back(m_CurrPage);
		m_CurrPage = nullptr;
		m_CurrOffset = 0;

		sm_PageManager[m_AllocationType].DiscardPages(FenceId, m_RetiredPages);
		m_RetiredPages.clear();

		sm_PageManager[m_AllocationType].FreeLargePages(FenceId, m_LargetPageList);
		m_LargetPageList.clear();
	}

	void LinearAllocator::DestroyAll()
	{
		sm_PageManager[0].Destroy();
		sm_PageManager[1].Destroy();
	}

	Pixel::DynAlloc LinearAllocator::AllocateLargePage(size_t SizeInBytes)
	{
		LinearAllocationPage* OneOff = sm_PageManager[m_AllocationType].CreateNewPage(SizeInBytes);
		m_LargetPageList.push_back(OneOff);

		DynAlloc ret(*OneOff, 0, SizeInBytes);
		ret.DataPtr = OneOff->m_CpuVirtualAddress;
		ret.GpuAddress = OneOff->m_GpuVirtualAddress;

		return ret;
	}
	//------Linear Allocator------
}
