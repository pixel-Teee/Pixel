#pragma once



#include <queue>

namespace Pixel {
	class CommandAllocatorPool
	{
	public:
		CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type);
		~CommandAllocatorPool();

		void ShutDown();

		//from the m_AlloactorPool to extract one alloactor
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> RequestAlloactor(uint64_t CompletedFenceValue);

		//TODO:need to figuare out this function
		void DiscardAlloactor(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Alloactor);

		size_t Size() { return m_AlloactorPool.size(); }
	private:
		//c:class
		//used for command allocator
		const D3D12_COMMAND_LIST_TYPE m_CommandListType;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_AlloactorPool;

		//pair's first is fence value
		std::queue <std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>> m_ReadyAlloactors;
		std::mutex m_AlloactorMutex;
	};

	class CommandQueue {
	public:
		friend class CommandListManager;

		CommandQueue(D3D12_COMMAND_LIST_TYPE Type);

		~CommandQueue();

		//create ID3D12CommandQueue
		void Create();

		void ShutDown();

		//this is we need to use, flush command queue
		void WaitForIdle() { WaitForFence(IncrementFence()); }

		void StallForFence(uint64_t FenceValue);
		void StallForProducer(CommandQueue& Producer);

		bool IsFenceComplete(uint64_t FenceValue);

		uint64_t GetNextFenceValue() { return m_NextFenceValue; }

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetNativeCommandQueue() { return m_pCommandQueue; }

		uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12CommandList> List);

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> RequesetAlloactor();

		void DiscardAlloactor(uint64_t FenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Alloactor);
	private:
		
		uint64_t IncrementFence();
		//parameter is we need to set fence value
		void WaitForFence(uint64_t FenceValue);

		std::mutex m_FenceMutex;
		std::mutex m_EventMutex;

		//Command Queue
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;

		const D3D12_COMMAND_LIST_TYPE m_Type;

		CommandAllocatorPool m_pAlloactorPool;

		//life time of these objects is managed by the descriptor cache
		Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
		//every frame, will increment this
		uint64_t m_NextFenceValue;
		uint64_t m_LastCompletedFenceValue;
		HANDLE m_FenceEventHandle;
	};

	class CommandListManager {
	public:
		CommandListManager();
		~CommandListManager();

		void Create();

		void ShutDown();

		//second parameter and third parameter is what need to create
		void CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& List, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& Allocator);

		//------Get Qeue------
		CommandQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
		CommandQueue& GetComputeQueue() { return m_ComputeQueue; }
		CommandQueue& GetCopyQueue() { return m_CopyQueue; }

		CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetNativeGraphicsCommandQueue() { return m_GraphicsQueue.GetNativeCommandQueue(); }
		//------Get Qeue------

		bool IsFenceComplete(uint64_t FenceValue);

		//cpu will wait for a fence to reach a specified value, this we will use
		void WaitForFence(uint64_t FenceValue);

		void IdleGPU();
	private:

		//three type command queue
		CommandQueue m_GraphicsQueue;
		CommandQueue m_ComputeQueue;
		CommandQueue m_CopyQueue;
	};
}