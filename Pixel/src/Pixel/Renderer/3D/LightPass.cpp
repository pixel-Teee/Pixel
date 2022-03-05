#include "pxpch.h"

#include "LightPass.h"

namespace Pixel
{

	void LightPass::SetShader(Ref<Shader> shader)
	{
		m_Shader = shader;
	}

	void LightPass::SetFramebuffer(Ref<Framebuffer> framebuffer)
	{
		m_Framebuffer = framebuffer;
	}

	uint32_t LightPass::GetFramebufferAttachment(AttachmentFormat attachment, uint32_t id /*= 0*/)
	{
		switch (attachment)
		{
		case Pixel::Color:
			return m_Framebuffer->GetColorAttachmentRendererID(id);
			break;
		case Pixel::Depth:
			return m_Framebuffer->GetDepthAttachmentRendererID();
			break;
		}

		PX_CORE_ASSERT(false, "Unknown AttachmentFormat");
		return 0;
	}

	void LightPass::Bind()
	{
		m_Framebuffer->Bind();
	}

	void LightPass::UnBind()
	{
		m_Framebuffer->UnBind();
	}

}