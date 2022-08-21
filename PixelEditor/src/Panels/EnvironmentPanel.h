#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class GraphNodeEditor;

	class EnvironmentPanel
	{
	public:
		EnvironmentPanel();
		~EnvironmentPanel();

		void OnImGuiRender();
	private:

		//------test------
		bool m_OpenGraphEditor = false;
		//------test------

		float m_Exposure = 1.0f;
		float m_BloomSigmma = 2.5f;
		Ref<DescriptorHandle> m_HDRTextureHandle;
	};
}
