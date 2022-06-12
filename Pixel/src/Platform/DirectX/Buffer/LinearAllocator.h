#pragma once

#include "Platform/DirectX/Buffer/DirectXGpuResource.h"

#include <queue>

namespace Pixel {

	#define DEFAULT_ALIGN 256
	struct DynAlloc
	{
		DynAlloc(DirectXGpuResource& BaseResource, size_t ThisOffset, size_t ThisSize)
		:Buffer(BaseResource), Offset(ThisOffset), Size(ThisSize) {}

		DirectXGpuResource& Buffer; //d3d buffer associated with this memory

		size_t Offset;//offset from start of buffer resource

		size_t Size;//reserved size of this allocation

		void* DataPtr;//cpu-writeable address
		D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;//the gpu-visible address
	};

	class LinearAllocationPage : public DirectXGpuResource
	{
	public:
		LinearAllocationPage(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Usage);

		~LinearAllocationPage();

		void Map();

		void UnMap();

		void* m_CpuVirtualAddress;
		D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
	};

	enum LinearAllocatorType
	{
		kInvalidAllocator = -1,
		kGpuExclusive = 0,//default gpu-writeable(via UAV)
		kCpuWritable = 1,//upload cpu-writeable(but write combined)
		kNumAllocatorTypes
	};

	enum 
	{
		kGpuAllocatorPageSize = 0x10000, //64K
		kCpuAllocatorPageSize = 0x20000 //2MB
	};

	class Device;
	class LinearAllocatorPageManager
	{
	public:
		LinearAllocatorPageManager();
		LinearAllocationPage* RequestPage();
		LinearAllocationPage* CreateNewPage(size_t PageSize);

		//discarded pages will get recyled. this is for fixed size pages
		void DiscardPages(uint64_t FenceId, const std::vector<LinearAllocationPage*>& Pages);

		//freed pages will be destroyed once their fence has passed
		//this is for signle-use, "large" pages.
		void FreeLargePages(uint64_t FenceId, const std::vector<LinearAllocationPage*> Pages);

		void Destroy();

	private:
		static LinearAllocatorType sm_AutoType;

		LinearAllocatorType m_AllocationType;
		//all page in this vector, we just need to destroy this
		std::vector<Scope<LinearAllocationPage>> m_PagePool;
		std::queue<std::pair<uint64_t, LinearAllocationPage*>> m_RetriedPages;
		std::queue<std::pair<uint64_t, LinearAllocationPage*>> m_DeletionQueue;
		std::queue<LinearAllocationPage*> m_AvailablePages;
		std::mutex m_Mutex;
	};

	class LinearAllocator
	{
	public:
		LinearAllocator(LinearAllocatorType Type);

		DynAlloc Allocate(size_t SizeInBytes, size_t Alignment);

		void CleanupUsedPages(uint64_t FenceId);

		static void DestroyAll();
	private:
		DynAlloc AllocateLargePage(size_t SizeInBytes);

		static LinearAllocatorPageManager sm_PageManager[2];

		LinearAllocatorType m_AllocationType;

		//m_PageSize:kGpuAllocatorPageSize or kCpuAllocatorPageSize
		size_t m_PageSize;
		size_t m_CurrOffset;

		LinearAllocationPage* m_CurrPage;

		std::vector<LinearAllocationPage*> m_RetiredPages;
		std::vector<LinearAllocationPage*> m_LargetPageList;
	};
}