#pragma once

#include "RendererPass.h"

namespace Pixel
{
	class LightPass : RendererPass
	{

	public:
		virtual void SetShader(Ref<Shader> shader) override;


		virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;


		virtual uint32_t GetFramebufferAttachment(AttachmentFormat attachment, uint32_t id = 0) override;


		virtual void Bind() override;


		virtual void UnBind() override;

	private:
		Ref<Shader> m_Shader;
		Ref<Framebuffer> m_Framebuffer;
	};
}
