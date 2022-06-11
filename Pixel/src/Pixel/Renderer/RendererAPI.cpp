#include "pxpch.h"
#include "RendererAPI.h"

namespace Pixel {
#ifdef PX_OPENGL
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#else
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX12;
#endif
}