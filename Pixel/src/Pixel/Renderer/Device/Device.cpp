#include "pxpch.h"

#include "Device.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	Device::~Device()
	{

	}

	Ref<Device> Device::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDevice>();
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}