#include "pxpch.h"
#include "ContentBrowserPanel.h"

#include "Pixel/Scene/SerializerMaterial.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"

//------my library------
#include "Pixel/Asset/AssetManager.h"
#include "Pixel/Utils/PlatformUtils.h"
//------my library------

//------other library------
#include <imgui/imgui.h>
//------other library------

namespace Pixel {

	//Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
	:m_CurrentDirectory(g_AssetPath)
	{
		m_Directory = Texture2D::Create(g_AssetPath.string() + "/icons/directory.png");
		m_File = Texture2D::Create(g_AssetPath.string() + "/icons/file.png");

		//copy the these thexture's descriptor to descriptor heap
		m_DirectoryHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		m_FileHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		
		Device::Get()->CopyDescriptorsSimple(1, m_DirectoryHandle->GetCpuHandle(), m_Directory->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_FileHandle->GetCpuHandle(), m_File->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
	}

	void ContentBrowserPanel::OpenAssetEditor(const std::string& filename)
	{
		Ref<Material> pMaterial;
		Ref<MaterialInstance> pMaterialInstance;
		SerializerMaterial seralizer;
		seralizer.DeserializerMaterialAssetAndCreateMaterial(filename, pMaterial, pMaterialInstance);

		if (pMaterial != nullptr)
		{
			m_bIsOpen = true;
			//Open The Node Graph
			m_NodeGraph = CreateRef<NodeGraph>(pMaterial, pMaterialInstance, filename);
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (m_NodeGraph != nullptr && m_bIsOpen)
		{
			m_NodeGraph->OnImGuiRender();
		}

		//list all the files in the assets directory
		ImGui::Begin("Content Browser");
		
		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		if (ImGui::IsMouseClicked(1) && ImGui::IsWindowFocused())
			ImGui::OpenPopup("Content Browser");

		if (ImGui::BeginPopup("Content Browser"))
		{
			if (ImGui::MenuItem("Create Material Asset"))
			{
				CreateMaterialAsset(m_CurrentDirectory.string());
			}

			if (ImGui::MenuItem("Import Model"))
			{
				//create model
			}

			if (ImGui::MenuItem("Import Texture"))
			{
				std::wstring filePath = FileDialogs::OpenFile(L"texture(*.jpg)\0*.jpg\0texture(*.png)\0*.png\0");
				AssetManager::GetSingleton().AddTextureToAssetRegistry(filePath);
			}
			ImGui::EndPopup();
		}

		float ContentWidth = ImGui::GetContentRegionAvailWidth();

		static float Padding = 16.0f;
		static float ThumbnailSize = 128.0f;

		int CellNumber = ContentWidth / (Padding + ThumbnailSize);

		if(CellNumber < 1)
			CellNumber = 1;

		ImGui::Columns(CellNumber, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{		
			const auto& path = directoryEntry.path();
			//ImGui::Text("%s", path.string().c_str());
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			//ImGui::Text("%s", relativePath.string().c_str());
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			bool isDirectory = directoryEntry.is_directory();
			bool isInAssetRegistry = AssetManager::GetSingleton().IsInAssetRegistry(relativePath.string());

			if (isDirectory)
			{
				//check this whether is directory or file?
				Ref<Texture2D> icon = directoryEntry.is_directory() ? m_Directory : m_File;

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ImageButton((ImTextureID)(m_DirectoryHandle->GetGpuHandle()->GetGpuPtr()), { ThumbnailSize, ThumbnailSize }, { 0, 1 }, { 1, 0 });
				ImGui::PopStyleColor();

				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
					{
						m_CurrentDirectory /= path.filename();
					}

					//------Open Asset Editor------
					if (!directoryEntry.is_directory())
					{
						OpenAssetEditor(path.string());
					}
					//------Open Asset Editor------
				}

				ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::NextColumn();
			}
			else if (!isDirectory && isInAssetRegistry)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ImageButton((ImTextureID)(m_FileHandle->GetGpuHandle()->GetGpuPtr()), { ThumbnailSize, ThumbnailSize }, { 0, 1 }, { 1, 0 });
				ImGui::PopStyleColor();

				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}

				if (isInAssetRegistry)
					ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::NextColumn();
			}

			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("ThumbnailSize", &ThumbnailSize, 16.0f, 512.0f);
		ImGui::SliderFloat("Padding", &Padding, 2.0f, 16.0f);

		ImGui::End();
	}

	void ContentBrowserPanel::CreateMaterialAsset(std::string filePath)
	{
		Ref<Material> pMaterial = CreateRef<Material>("Material");
		Ref<MaterialInstance> pMaterialInstance = CreateRef<MaterialInstance>(pMaterial);

		filePath += "\\Test.mut";

		SerializerMaterial serializerMaterial;
		serializerMaterial.SerializerMaterialAsset(filePath, pMaterial, pMaterialInstance);
	}

}