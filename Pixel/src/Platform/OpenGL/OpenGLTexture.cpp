#include "pxpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Pixel {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):m_path(path)
	{
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		PX_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 1)
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		PX_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		//不想让texture的数据继续保存在cpu的内存上
		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat textureFormat):m_Width(width), m_Height(height)
	{
		if (textureFormat == TextureFormat::RGBA)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (textureFormat == TextureFormat::RGB)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}
		else if (textureFormat == TextureFormat::RED)
		{
			m_InternalFormat = GL_R8;
			m_DataFormat = GL_RED;
		}

		//PX_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp; 
		if (m_DataFormat == GL_RGB)
		{
			bpp = 3;
		}
		else if (m_DataFormat == GL_RGBA)
		{
			bpp = 4;
		}
		else bpp = 1;
		PX_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		//glActiveTexture(GL_TEXTURE0 + slot);
		glBindTextureUnit(slot, m_RendererID);
		//glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

}