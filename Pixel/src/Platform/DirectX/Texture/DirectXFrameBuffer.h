#pragma once

#include "Pixel/Renderer/Framebuffer.h"

#include <vector>

namespace Pixel {
	class DirectXColorBuffer;
	class DepthBuffer;
	class DirectXFrameBuffer : public Framebuffer
	{
	public:
		DirectXFrameBuffer(const FramebufferSpecification& spec);

		virtual ~DirectXFrameBuffer();

		//invalidate or resize
		void Invalidate();

		virtual void Bind() override;

		virtual void UnBind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetRenderId() override;

		virtual uint64_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		virtual uint32_t GetDepthAttachmentRendererID() const override;

		virtual void SetDepthAttachmentRendererID(uint32_t rendererID) override;

		virtual const FramebufferSpecification& GetSpecification() const override;

		virtual void CloseColorAttachmentDraw() override;

		virtual void SetColorAttachmentDraw(uint32_t index) override;

	private:
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;

		std::vector<Ref<DirectXColorBuffer>> m_pColorBuffers;
		Ref<DepthBuffer> m_pDepthBuffer;
	};
}
