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
		virtual bool Resize(uint32_t width, uint32_t height) override;
	};
}
