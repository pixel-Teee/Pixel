#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class EnvironmentPanel
	{
	public:
		EnvironmentPanel();
		~EnvironmentPanel();

		void SetSkyBox(Ref<CubeMap> skyBox);
		void SetVisualizeFacesTextures(const std::vector<std::string>& paths);
		void SetVisualizeFacesTexture(FaceTarget faceIndex, std::string& path);

		std::string& GetPath(FaceTarget faceIndex);
		void OnImGuiRender();
	private:
		//from scene
		Ref<CubeMap> m_skyBox;

		std::vector<std::string> m_paths;

		//Visualization
		Ref<Texture> m_right;
		Ref<Texture> m_left;
		Ref<Texture> m_top;
		Ref<Texture> m_bottom;
		Ref<Texture> m_back;
		Ref<Texture> m_front;
	};
}
