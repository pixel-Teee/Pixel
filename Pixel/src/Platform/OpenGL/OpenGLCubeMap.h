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

		virtual void Bind(uint32_t slot) override;
		virtual void UnBind() override;

		virtual void GenerateCubeMap() override;

		virtual std::vector<std::string>& GetPaths() override;

		//info the editor
		virtual void SetDirty(bool value) override;
		virtual bool IsDirty() override;

	private:
		uint32_t m_RendererID;
		bool m_IsDirty = false;
		//6 faces' path
		std::vector<std::string> m_paths;
	};
}
