#include "pxpch.h"

#include "DirectXContextManager.h"
#include "Platform/DirectX/TypeUtils.h"

#include "GraphicsContext.h"

namespace Pixel {

	DirectXContextManager::~DirectXContextManager()
	{
		DestroyAllContexts();
	}

	Ref<Context> DirectXContextManager::AllocateContext(CommandListType CmdListType, Ref<Device> pDevice)
	{
		D3D12_COMMAND_LIST_TYPE DirectXCmdListType = CmdListTypeToDirectXCmdListType(CmdListType);

		std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);

		auto& AvaiableContext = sm_AvailableContexts[DirectXCmdListType];

		Ref<Context> ReturnContext;
		if (AvaiableContext.empty())
		{
			//Test
			//Ref<GraphicsContext> Test = std::make_shared<GraphicsContext>(CmdListType, shared_from_this());
			//if(CmdListType == CommandListType::Graphics)
				//ReturnContext = std::make_shared<GraphicsContext>(CmdListType, shared_from_this());
			ReturnContext = std::make_shared<GraphicsContext>(CmdListType, shared_from_this(), pDevice);
			sm_ContextPool[DirectXCmdListType].emplace_back(ReturnContext);
			ReturnContext->Initialize(pDevice);
		}
		else
		{
			ReturnContext = AvaiableContext.front();
			AvaiableContext.pop();
			ReturnContext->Reset(pDevice);
		}

		return ReturnContext;
	}

	void DirectXContextManager::FreeContext(Ref<Context> UsedContext)
	{
		std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);

		//Ref<DirectXContext> pDirectXContext = std::static_pointer_cast<DirectXContext>(UsedContext);

		sm_AvailableContexts[CmdListTypeToDirectXCmdListType(UsedContext->GetType())].push(UsedContext);
	}

	void DirectXContextManager::DestroyAllContexts()
	{
		for (size_t i = 0; i < 4; ++i)
		{
			sm_ContextPool[i].clear();
		}
	}

}