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

		Ref<DescriptorHandle> m_HDRTextureHandle;
	};
}
