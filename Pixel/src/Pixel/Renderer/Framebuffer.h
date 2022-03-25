#pragma once

#include "Pixel/Core/Core.h"

namespace Pixel {
	enum class FramebufferTextureFormat
	{
		None = 0,

		//Color
		RGBA8,
		RGBA16F,
		RED_INTEGER,

		//Depth/Stencil
		DEPTH24STENCIL8,

		//Defauls
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
		:TextureFormat(format){};

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		//TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification> attachments)
		:Attachments(attachments){}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		//FramebufferFormat Format = 
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	void BindWriteFramebuffer(uint32_t renderId);
	void BindReadFramebuffer(uint32_t renderId);
	void BlitFramebuffer(int32_t srcWidth, int32_t srcHeight);

	class Framebuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetRenderId() = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual void SetDepthAttachmentRendererID(uint32_t rendererID) = 0;

		//virtual FramebufferSpecification& GetSpecification() = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		virtual void CloseColorAttachmentDraw() = 0;
		virtual void SetColorAttachmentDraw(uint32_t index) = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
