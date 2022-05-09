#include "pxpch.h"
#include "EnvironmentPanel.h"

#include <filesystem>

#include <Imgui/imgui.h>

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;
	EnvironmentPanel::EnvironmentPanel()
	{
		//m_skyBox = CubeMap::Create();

		m_paths = std::vector<std::string>(6);
		m_right = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_left = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_top = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_bottom = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_back = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_front = Texture2D::Create(64, 64, TextureFormat::RGB);
	}

	void EnvironmentPanel::SetVisualizeFacesTextures(const std::vector<std::string>& paths)
	{
		m_right = Texture2D::Create(paths[0]);
		m_left = Texture2D::Create(paths[1]);
		m_top = Texture2D::Create(paths[2]);
		m_bottom = Texture2D::Create(paths[3]);
		m_back = Texture2D::Create(paths[4]);
		m_front = Texture2D::Create(paths[5]);
	}

	void EnvironmentPanel::SetVisualizeFacesTexture(FaceTarget faceIndex, std::string& path)
	{
		switch (faceIndex)
		{
			case FaceTarget::Right:
				m_paths[0] = path;
				m_right = Texture2D::Create(path);
				break;

			case FaceTarget::Left:
				m_paths[1] = path;
				m_left = Texture2D::Create(path);
				break;

			case FaceTarget::Top:
				m_paths[2] = path;
				m_top = Texture2D::Create(path);
				break;

			case FaceTarget::Bottom:
				m_paths[3] = path;
				m_bottom = Texture2D::Create(path);
				break;

			case FaceTarget::Back:
				m_paths[4] = path;
				m_back = Texture2D::Create(path);
				break;

			case FaceTarget::Front:
				m_paths[5] = path;
				m_front = Texture2D::Create(path);
				break;
		}
	}

	std::string& EnvironmentPanel::GetPath(FaceTarget faceIndex)
	{
		return m_paths[(uint32_t)faceIndex];
	}

	EnvironmentPanel::~EnvironmentPanel()
	{

	}

	void EnvironmentPanel::SetSkyBox(Ref<CubeMap> skyBox)
	{
		m_skyBox = skyBox;
	}

	void EnvironmentPanel::OnImGuiRender()
	{
		ImGui::Begin("Environment Settings");
		//Sky Box
		if (ImGui::TreeNode("SkyBox"))
		{
			ImGui::Text("Right +X");
			ImGui::Image((ImTextureID)m_right->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[0] = texturePath.string();
					m_right = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Right, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Left -X");
			ImGui::Image((ImTextureID)m_left->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[1] = texturePath.string();
					m_left = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Left, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Top +Y");
			ImGui::Image((ImTextureID)m_top->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[2] = texturePath.string();
					m_top = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Top, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Bottom -Y");
			ImGui::Image((ImTextureID)m_bottom->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[3] = texturePath.string();
					m_bottom = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Bottom, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Back +Z");
			ImGui::Image((ImTextureID)m_back->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[4] = texturePath.string();
					m_back = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Back, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Front -Z");
			ImGui::Image((ImTextureID)m_front->GetRendererID(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					m_paths[5] = texturePath.string();
					m_front = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Front, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 178.0f / 255.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 111.0f / 255.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 198.0f / 255.0f));
			if (ImGui::Button("Generate CubeMap"))
			{
				m_skyBox->GenerateCubeMap();
			}
			ImGui::PopStyleColor(3);
			ImGui::TreePop();
		}
		ImGui::End();
	}

}
