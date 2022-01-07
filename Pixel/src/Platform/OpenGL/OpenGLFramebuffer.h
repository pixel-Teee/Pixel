#pragma once

#include "Pixel/Renderer/Framebuffer.h"

namespace Pixel {

	class OpenGLFramebuffer : public Framebuffer 
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		//void Invalidate or resize
		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		//Color Attachment
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override { return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification;}
	private:
		uint32_t m_RendererID = 0;
		
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;

		//every render target's opengl id
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
