#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class EnvironmentPanel
	{
	public:
		EnvironmentPanel();
		~EnvironmentPanel();

		void OnImGuiRender();
	private:
		float m_Exposure = 1.0f;
		Ref<DescriptorHandle> m_HDRTextureHandle;
	};
}
