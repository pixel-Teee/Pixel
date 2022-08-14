#pragma once

#include <filesystem>

#include "Pixel/Renderer/Texture.h"
#include "../NodeGraph/NodeGraph.h"

namespace Pixel {
	class DescriptorHandle;
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		void CreateMaterialAsset(std::string filePath);

	private:
		std::filesystem::path m_CurrentDirectory;

		/*------
		Node Graph
		-------*/

		Ref<Texture2D> m_Directory;
		Ref<Texture2D> m_File;

		Ref<DescriptorHandle> m_DirectoryHandle;
		Ref<DescriptorHandle> m_FileHandle;
	};
}
