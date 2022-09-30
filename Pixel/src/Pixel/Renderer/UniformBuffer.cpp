#include "pxpch.h"
#include "UniformBuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Pixel {
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t Offset, uint32_t BufferSize, uint32_t BindIndex)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLUniformBuffer>(Offset, BufferSize, BindIndex);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
