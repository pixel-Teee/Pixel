#include "pxpch.h"
#include "EnvironmentPanel.h"

#include <filesystem>

#include <Imgui/imgui.h>

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;
	EnvironmentPanel::EnvironmentPanel()
	{
		//m_skyBox = CubeMap::Create();

		m_right = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_left = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_top = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_bottom = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_back = Texture2D::Create(64, 64, TextureFormat::RGB);
		m_front = Texture2D::Create(64, 64, TextureFormat::RGB);
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
					m_front = Texture2D::Create(texturePath.string());
					m_skyBox->SetFace(FaceTarget::Front, texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}
			
			ImGui::TreePop();
		}
		ImGui::End();
	}

}
