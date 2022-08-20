#pragma once

#include <filesystem>

#include "Pixel/Renderer/Texture.h"
#include "../GraphNode/GraphNodeEditor.h"

namespace Pixel {
	class DescriptorHandle;
	struct SubMaterial;
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		void CreateMaterialAsset(std::string filePath);

	private:
		void RenderMaterialAssetPanel();

		std::filesystem::path m_CurrentDirectory;

		/*------
		Node Graph
		-------*/

		Ref<Texture2D> m_Directory;
		Ref<Texture2D> m_File;

		Ref<DescriptorHandle> m_DirectoryHandle;
		Ref<DescriptorHandle> m_FileHandle;

		//------current already open sub material------
		Ref<DescriptorHandle> m_MaterialAssetTextureHandle;
		Ref<SubMaterial> m_pSubMaterial;
		std::string m_CurrentSubMaterialPath;//current open sub material physical path
		bool m_IsOpen;
		//------current already open sub material------
	};
}
