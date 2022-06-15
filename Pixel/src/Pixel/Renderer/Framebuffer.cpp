#include "pxpch.h"
#include "Framebuffer.h"

#include "Pixel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/DirectX/Texture/DirectXFrameBuffer.h"

namespace Pixel {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
			case RendererAPI::API::DirectX12: return CreateRef<DirectXFrameBuffer>(spec);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");

		return nullptr;
	}

}