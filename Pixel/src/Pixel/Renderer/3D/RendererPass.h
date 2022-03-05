#pragma once

#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/Framebuffer.h"

namespace Pixel
{
	enum AttachmentFormat
	{
		Color = 0,
		Depth = 1
	};

	enum RenderType
	{
		EForwardPass = 0,
		EGeometryPass = 1,
		ELightPass = 2,
		EShaowPass = 3
	};

	class RendererPass
	{
	public:
		RendererPass();

		virtual void SetShader(Ref<Shader> shader) = 0;
		virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) = 0;

		virtual uint32_t GetFramebufferAttachment(AttachmentFormat attachment, uint32_t id = 0) = 0;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		//Create SubClass
		template<typename ...Args>
		static Ref<RendererPass> CreateRendererPass(RenderType type, Args&& ...args)
		{
			switch (type)
			{
				case RenderType::ELightPass:
					return Ref<LightPass>(std::forward<Args>(args)...);
				case RenderType::EGeometryPass:
					return Ref<GeometryPass>(std::forward<Args>(args)...);
			}
			return nullptr;
		}
	};
}
