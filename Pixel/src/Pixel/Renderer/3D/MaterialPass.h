#pragma once

#include "RendererPass.h"

namespace Pixel
{
	class MaterialPass : public RenderPass
	{
	public:
		MaterialPass();
		virtual ~MaterialPass();
		virtual bool Draw() override;
		virtual RenderPassType GetPassType() override;
	};
}
