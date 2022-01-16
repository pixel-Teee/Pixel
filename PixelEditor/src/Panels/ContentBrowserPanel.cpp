#include "pxpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Pixel {

	//Once we have projects, change this
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
	:m_CurrentDirectory(g_AssetPath)
	{
		m_Directory = Texture2D::Create(g_AssetPath.string() + "/icons/directory.png");
		m_File = Texture2D::Create(g_AssetPath.string() + "/icons/file.png");
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
			//Check this whether is directory or file?
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_Directory : m_File; 

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {ThumbnailSize, ThumbnailSize}, {0, 1}, {1, 0});
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
			}
			ImGui::TextWrapped("%s", filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("ThumbnailSize", &ThumbnailSize, 16.0f, 512.0f);
		ImGui::SliderFloat("Padding", &Padding, 2.0f, 16.0f);

		ImGui::End();
	}

}