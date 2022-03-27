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

		void OnImGuiRender();
	private:
		Ref<CubeMap> m_skyBox;

		//Visualization
		Ref<Texture> m_right;
		Ref<Texture> m_left;
		Ref<Texture> m_top;
		Ref<Texture> m_bottom;
		Ref<Texture> m_back;
		Ref<Texture> m_front;
	};
}
