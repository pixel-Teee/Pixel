#pragma once

#include "Pixel/Renderer/Texture.h"

namespace Pixel {

	class OpenGLCubeMap : public CubeMap
	{
	public:
		OpenGLCubeMap();
		OpenGLCubeMap(std::vector<std::string>& paths);
		virtual void SetFace(FaceTarget faceIndex, const std::string& path);
		~OpenGLCubeMap();

		virtual void Bind() override;
		virtual void UnBind() override;

	private:
		bool is_Initialized = false;
		uint32_t m_RendererID;
	};
}
