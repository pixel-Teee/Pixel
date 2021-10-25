#include "pxpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Pixel {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}