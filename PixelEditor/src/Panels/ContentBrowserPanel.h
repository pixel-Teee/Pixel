#pragma once

#include <filesystem>

#include "Pixel/Renderer/Texture.h"
#include "../GraphNode/GraphNodeEditor.h"

namespace Pixel {
	class DescriptorHandle;
	struct SubMaterial;
	class Material;
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		void CreateMaterialAsset(std::string filePath);

		//void SetGraphNodeEditorPreviewSceneFrameBuffer(Ref<Framebuffer> pFinalFrameBuffer);

		void RegisterOpenGraphEditorCallBack(std::function<void(const std::string& virtualPath, const std::string& physicalPath, Ref<Material> pMaterial)> func);

		void RegisterIsGraphEditorAliveCallBack(std::function<void(bool)> func);

		void RegisterGenerateThumbNail(std::function<void(Ref<Material> pMaterial)> func);

		void RegisterOpenMaterialInstanceEditor(std::function<void(const std::string&)> func);

		void CreateMaterialInstance(std::string& virtualPath);
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

		//------current open test material------
		Ref<Material> m_pMaterial;
		std::string m_CurrentTestMaterialPath;
		//bool m_IsOpenTestMaterialEditor;
		Ref<GraphNodeEditor> m_GraphNodeEditor;
		//------current open test material------

		//------material asset texture preview image handle------
		Ref<DescriptorHandle> m_MaterialAssertTextureFirstHandle;
		uint32_t m_CurrentMaterialAssertTextureHandleOffset;
		bool m_DelayToLoadImageNextFrame;
		//------material asset texture preview image handle------

		Ref<Framebuffer> m_pFramebuffer;

		std::function<void(const std::string& virtualPath, const std::string& physicalPath, Ref<Material> pMaterial)> m_OpenGraphEditor;

		std::function<void(bool)> m_IsGraphEditorAlive;

		std::function<void(Ref<Material> pMaterial)> m_GenerateThumbNail;

		std::function<void(const std::string&)> m_OpenMaterialInstanceEditor;//open material instance editor
	};
}
