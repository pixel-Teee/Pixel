#include "pxpch.h"

#include "DirectXContextManager.h"
#include "DirectXContext.h"
#include "Platform/DirectX/d3dx12.h"

namespace Pixel {

	D3D12_COMMAND_LIST_TYPE DirectXContextManager::CommandListTypeToDirectXCommandListType(CommandListType CmdListType)
	{
		switch (CmdListType)
		{
		case CommandListType::Graphics:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		case CommandListType::Compute:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case CommandListType::Copy:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			PX_CORE_ASSERT(false, "unkonwn context type!");
			break;
		}
	}

	Ref<Context> DirectXContextManager::AllocateContext(CommandListType CmdListType)
	{
		D3D12_COMMAND_LIST_TYPE DirectXCmdListType = CommandListTypeToDirectXCommandListType(CmdListType);

		std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);

		auto& AvaiableContext = sm_AvailableContexts[DirectXCmdListType];

		Ref<DirectXContext> ReturnContext;
		if (AvaiableContext.empty())
		{
			ReturnContext = CreateRef<DirectXContext>(CmdListType, shared_from_this());
			sm_ContextPool[DirectXCmdListType].emplace_back(ReturnContext);
			ReturnContext->Initialize();
		}
		else
		{
			ReturnContext = AvaiableContext.front();
			AvaiableContext.pop();
			ReturnContext->Reset();
		}

		return ReturnContext;
	}

	void DirectXContextManager::FreeContext(Ref<Context> UsedContext)
	{
		std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);

		Ref<DirectXContext> pDirectXContext = std::static_pointer_cast<DirectXContext>(UsedContext);

		sm_AvailableContexts[pDirectXContext->m_Type].push(pDirectXContext);
	}

	void DirectXContextManager::DestroyAllContexts()
	{
		for (size_t i = 0; i < 4; ++i)
		{
			sm_ContextPool[i].clear();
		}
	}

}