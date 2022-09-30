#pragma once

#include <filesystem>

#include "Pixel/Renderer/Texture.h"
#include "../NodeGraph/NodeGraph.h"

namespace Pixel {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OpenAssetEditor(const std::string& filename);

		void OnImGuiRender();

		void CreateMaterialAsset(std::string filePath);

	private:
		std::filesystem::path m_CurrentDirectory;

		/*------
		Node Graph
		-------*/

		bool m_bIsOpen = false;
		Ref<NodeGraph> m_NodeGraph;

		Ref<Texture2D> m_Directory;
		Ref<Texture2D> m_File;
	};
}
