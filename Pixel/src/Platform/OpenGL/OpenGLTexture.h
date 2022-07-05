#pragma once

#include "Pixel/Renderer/Texture.h"

#include <glad/glad.h>

namespace Pixel {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat textureFormat);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width;}
		virtual uint32_t GetHeight() const override { return m_Height;}
		virtual uint64_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

		virtual std::string& GetPath() override;

		virtual Ref<DescriptorCpuHandle> GetCpuDescriptorHandle() override;

		virtual Ref<DescriptorHandle> GetHandle() const override;

		virtual Ref<DescriptorHandle> GetRtvHandle() const override;

	private:
		std::string m_path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
