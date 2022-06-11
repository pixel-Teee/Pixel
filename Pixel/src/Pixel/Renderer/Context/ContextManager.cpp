#include "pxpch.h"

#include "ContextManager.h"
#include "Pixel/Renderer/Renderer.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"

namespace Pixel {

	ContextManager::~ContextManager()
	{

	}

	Ref<ContextManager> ContextManager::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXContextManager>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Context> ContextManager::CreateGraphicsContext(const std::wstring& ID, Ref<Device> pDevice)
	{
		Ref<Context> NewContext = AllocateContext(CommandListType::Graphics, pDevice);
		NewContext->SetID(ID);
		//------g_ContextManager's pool reference this context------
		return NewContext;
	}

}