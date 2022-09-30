#include "pxpch.h"
#include "OpenGLCubeMap.h"

#include <glad/glad.h>
#include "stb_image.h"

namespace Pixel {

	void OpenGLCubeMap::SetFace(FaceTarget faceIndex, const std::string& path)
	{
		uint32_t index;
		switch (faceIndex)
		{
			case FaceTarget::Right:
				index = 0;
				break;
			case FaceTarget::Left:
				index = 1;
				break;
			case FaceTarget::Top:
				index = 2;
				break;
			case FaceTarget::Bottom:
				index = 3;
				break;
			case FaceTarget::Back:
				index = 4;
				break;
			case FaceTarget::Front:
				index = 5;
				break;
		}
		m_paths[faceIndex] = path;
	}

	//default every face is white
	OpenGLCubeMap::OpenGLCubeMap()
	{
		//white texture
		unsigned char data[3];
		data[0] = 0xff;
		data[1] = 0xff;
		data[2] = 0xff;
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int32_t width = 1, height = 1;
		for (uint32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLCubeMap::OpenGLCubeMap(std::vector<std::string>& paths)
	{
		glDeleteTextures(1, &m_RendererID);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		m_paths = paths;

		for (uint32_t i = 0; i < m_paths.size(); ++i)
		{
			int32_t width, height, nrChannels;

			unsigned char* data = stbi_load(m_paths[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				PIXEL_CORE_ERROR("Cubemap don't loaded correctly!");
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMap::Bind(uint32_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubeMap::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void OpenGLCubeMap::GenerateCubeMap()
	{
		glDeleteTextures(1, &m_RendererID);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (uint32_t i = 0; i < m_paths.size(); ++i)
		{
			int32_t width, height, nrChannels;

			unsigned char* data = stbi_load(m_paths[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				PIXEL_CORE_ERROR("Cubemap don't loaded correctly!");
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	std::vector<std::string>& OpenGLCubeMap::GetPaths()
	{
		return m_paths;
	}

	void OpenGLCubeMap::SetDirty(bool value)
	{
		m_IsDirty = value;
	}

	bool OpenGLCubeMap::IsDirty()
	{
		return m_IsDirty;
	}

}