#include "pxpch.h"

#include "PixelBuffer.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Buffer/DirectXPixelBuffer.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"

namespace Pixel {

	Ref<PixelBuffer> PixelBuffer::CreatePixelBuffer()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12:
		{
			return std::make_shared<DirectXPixelBuffer>();
		}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<PixelBuffer> PixelBuffer::CreateColorBuffer(const glm::vec4& ClearColor)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12:
		{
			return std::make_shared<DirectXColorBuffer>(ClearColor);
		}
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//Ref<PixelBuffer> PixelBuffer::CreateDepthBuffer()
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	//	case RendererAPI::API::DirectX12:
	//	{
	//		//return std::make_shared<DepthBuffer>();
	//		Ref<DepthBuffer> pDepthBuffer = std::make_shared<DepthBuffer>();
	//		return std::static_pointer_cast<PixelBuffer>(pDepthBuffer);
	//	}
	//	}

	//	PX_CORE_ASSERT(false, "Unknown RendererAPI!");
	//	return nullptr;
	//}

}