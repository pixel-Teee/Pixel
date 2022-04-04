#pragma once
#include "RendererPass.h"

namespace Pixel
{
	class GeometryPass : public RenderPass {
	public:
		GeometryPass();
		virtual ~GeometryPass();

		virtual bool Resize(uint32_t width, uint32_t height) override;
	public:
		virtual bool Draw() override;
		virtual RenderPassType GetPassType() override;
	private:
		Ref<Framebuffer> m_GeometryFrameBuffer;
	};
}
