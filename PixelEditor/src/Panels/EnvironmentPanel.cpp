#include "pxpch.h"
#include "EnvironmentPanel.h"

#include <filesystem>

#include <Imgui/imgui.h>

#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "Pixel/Renderer/Device/Device.h"

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;
	EnvironmentPanel::EnvironmentPanel()
	{
		m_HDRTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		//------default hdr texture------
		std::string texturePath = "assets/textures/hdr/brown_photostudio_01_1k.hdr";
		//------default hdr texture------

		Application::Get().GetRenderer()->InitializeAndConvertHDRToCubeMap(texturePath);

		Device::Get()->CopyDescriptorsSimple(1, m_HDRTextureHandle->GetCpuHandle(), Application::Get().GetRenderer()->GetHDRDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
	}
	
	EnvironmentPanel::~EnvironmentPanel()
	{

	}

	void EnvironmentPanel::OnImGuiRender()
	{
		ImGui::Begin("Environment Settings");
		//Sky Box
		if (ImGui::TreeNode("SkyBox"))
		{
			ImGui::Text("HDR Environment");
			ImGui::Image((ImTextureID)m_HDRTextureHandle->GetGpuHandle()->GetGpuPtr(), ImVec2(64, 64));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Application::Get().GetRenderer()->InitializeAndConvertHDRToCubeMap(texturePath.string());
					Device::Get()->CopyDescriptorsSimple(1, m_HDRTextureHandle->GetCpuHandle(), Application::Get().GetRenderer()->GetHDRDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
				}
				ImGui::EndDragDropTarget();
			}
			
			//ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 178.0f / 255.0f));
			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 111.0f / 255.0f));
			//ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(51.0f / 255.0f, 197.0f / 255.0f, 198.0f / 255.0f));
			//if (ImGui::Button("Generate CubeMap"))
			//{
			//	Application::Get()->GetRenderer()->InitializeAndConvertHDRToCubeMap()
			//}
			//ImGui::PopStyleColor(3);
			ImGui::TreePop();
		}
		ImGui::End();
	}

}
