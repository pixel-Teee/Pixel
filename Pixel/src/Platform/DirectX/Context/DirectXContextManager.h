#pragma once

#include "Pixel/Renderer/Context/ContextManager.h"
#include "Platform/DirectX/d3dx12.h"

namespace Pixel {
	class DirectXContext;

	//enum class CommandListType;

	class DirectXContextManager : public ContextManager
	{
	public:
		~DirectXContextManager();
		virtual Ref<Context> AllocateContext(CommandListType CmdListType, Ref<Device> pDevice) override;
		virtual void FreeContext(Ref<Context> UsedContext) override;
		virtual void DestroyAllContexts() override;

		//static D3D12_COMMAND_LIST_TYPE CommandListTypeToDirectXCommandListType(CommandListType CmdListType);
	private:
		//three type, [0], [1](x), [2], [3]
		std::vector<Ref<Context>> sm_ContextPool[4];
		//three type, [0], [1](x), [2], [3]
		std::queue<Ref<Context>> sm_AvailableContexts[4];
		std::mutex sm_ContextAllocationMutex;
	};
}