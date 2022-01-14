#pragma once

#include <filesystem>

#include "Pixel/Renderer/Texture.h"

namespace Pixel {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_Directory;
		Ref<Texture2D> m_File;
	};
}
