#include "pxpch.h"
#include "OpenGLCubeMap.h"

#include <glad/glad.h>
#include "stb_image.h"

namespace Pixel {

	void OpenGLCubeMap::SetFace(FaceTarget faceIndex, const std::string& path)
	{
		if (!is_Initialized)
		{
			glGenTextures(1, &m_RendererID);
			is_Initialized = false;
		}
		
		glBindTexture(1, m_RendererID);

		int32_t width, height, nrChannels;
		
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		GLenum internalFormat, dataFormat;
		if (nrChannels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else
		{
			PIXEL_CORE_ERROR("Cubemap need 3 component!");
		}

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			PIXEL_CORE_ERROR("Cubemap don't loaded correctly!");
			stbi_image_free(data);
		}
	}

	//default every face is white
	OpenGLCubeMap::OpenGLCubeMap()
	{
		//white texture
		unsigned char data[3];
		data[0] = 0xff;
		data[1] = 0xff;
		data[2] = 0xff;
		if (!is_Initialized)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
			is_Initialized = false;

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
			is_Initialized = true;
		}
	}

	OpenGLCubeMap::OpenGLCubeMap(std::vector<std::string>& paths)
	{
		if (!is_Initialized)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
			is_Initialized = false;

			int32_t width = 1, height = 1;
			for (uint32_t i = 0; i < paths.size(); ++i)
			{
				int32_t width, height, nrChannels;

				unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
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
			is_Initialized = true;
		}
	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMap::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubeMap::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}