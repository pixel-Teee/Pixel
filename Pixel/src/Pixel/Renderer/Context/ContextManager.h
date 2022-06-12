#pragma once

#include <queue>

#include "ContextType.h"

namespace Pixel {
	class Context;
	class Device;
	//context managers's interface
	class ContextManager : public std::enable_shared_from_this<ContextManager>
	{
	public:
		virtual ~ContextManager();

		virtual Ref<Context> AllocateContext(CommandListType CmdListType) = 0;
		virtual void FreeContext(Ref<Context> UsedContext) = 0;
		virtual void DestroyAllContexts() = 0;

		static Ref<ContextManager> Create();

		Ref<Context> CreateGraphicsContext(const std::wstring& ID);
	};
}
