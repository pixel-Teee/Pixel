#include "pxpch.h"
#include "GraphicsContext.h"

namespace Pixel {

	GraphicsContext& GraphicsContext::Begin(const std::wstring& ID)
	{
		return DirectXContext::Begin(ID).GetGraphicsContext();
	}

}