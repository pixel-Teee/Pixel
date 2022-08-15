#include "pxpch.h"
#include "ContentBrowserPanel.h"

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

	void ContentBrowserPanel::OnImGuiRender()
	{
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
			/*if (ImGui::MenuItem("Create Material Asset"))
			{
				CreateMaterialAsset(m_CurrentDirectory.string());
			}*/

			if (ImGui::MenuItem("Import Model"))
			{
				//create model
				std::wstring filePath = FileDialogs::OpenFile(L"model(*.fbx)\0*.fbx\0model(*.obj)\0*.obj\0");
				AssetManager::GetSingleton().AddModelToAssetRegistry(AssetManager::GetSingleton().to_string(filePath));
			}

			if (ImGui::MenuItem("Import Texture"))
			{
				std::wstring filePath = FileDialogs::OpenFile(L"texture(*.jpg)\0*.jpg\0texture(*.png)\0*.png\0texture(*.hdr)\0*.hdr\0");
				AssetManager::GetSingleton().AddTextureToAssetRegistry(filePath);
			}

			if (ImGui::MenuItem("Create Material"))
			{
				std::wstring filePath = FileDialogs::SaveFile(L"material(*.mat)\0*.mat\0");

				//write a default material and add to asset registry
				AssetManager::GetSingleton().CreateSubMaterial(AssetManager::GetSingleton().to_string(filePath));

				AssetManager::GetSingleton().AddMaterialToAssetRegistry(filePath);
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
					//const wchar_t* itemPath = relativePath.c_str();
					const std::string& itemPath = relativePath.string();

					//query the asset virtual path
					const std::string& virtualPath = AssetManager::GetSingleton().GetAssetRegistryPath(itemPath);

					//pass the asset virtual path
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", virtualPath.c_str(), strlen(virtualPath.c_str()) * sizeof(char), ImGuiCond_Once);

					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
					{
						m_CurrentDirectory /= path.filename();
					}
					else
					{
						//check the file type, and open different editor

					}
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
					//const wchar_t* itemPath = relativePath.c_str();
					const std::string& itemPath = relativePath.string();

					//query the asset virtual path
					const std::string& virtualPath = AssetManager::GetSingleton().GetAssetRegistryPath(itemPath);

					//pass the asset virtual path
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", virtualPath.c_str(), (strlen(virtualPath.c_str()) + 1) * sizeof(char), ImGuiCond_Once);

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

}