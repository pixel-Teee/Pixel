#pragma once

#include "Pixel/Renderer/GraphicsContext.h"

namespace Pixel {
	class DirectXContext : public GraphicsContext
	{
	public:
		virtual void Init() override;
		virtual void SwapBuffers() override;
	};
}