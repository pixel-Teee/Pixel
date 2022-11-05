#include "pxpch.h"

#include "MaterialInstanceEditor.h"

#include "Pixel/Asset/AssetManager.h"
#include "Pixel/Renderer/3D/Material/MaterialInstance.h"
#include "Pixel/Renderer/3D/Material/CustomFloatValue.h"
#include "Pixel/Renderer/3D/Material/CustomTexture2D.h"
#include "Pixel/Scene/SceneSerializer.h"
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include "imgui/imgui.h"

namespace Pixel {
	MaterialInstanceEditor::MaterialInstanceEditor()
	{

	}

	MaterialInstanceEditor::MaterialInstanceEditor(const std::string& virtualPath)
	{
		//from virtual path to get material instance
		//get material instance
		m_pMaterialInstance = AssetManager::GetSingleton().GetMaterialInstance(virtualPath);
		m_MaterialInstanceVirtualPath = virtualPath;
	}

	MaterialInstanceEditor::~MaterialInstanceEditor()
	{

	}

	void MaterialInstanceEditor::OnImGuiRenderer(bool& OpenMaterialInstanceEditor)
	{
		ImGui::Begin("MaterialInstanceEditor", &OpenMaterialInstanceEditor);

		//editor material instance editor parameter
		if (m_pMaterialInstance)
		{
			for (size_t j = 0; j < m_pMaterialInstance->m_PSShaderCustomValue.size(); ++j)
			{
				ImGui::Text(m_pMaterialInstance->m_PSShaderCustomValue[j]->ConstValueName.c_str());//variable name
				ImGui::SameLine();
				if (m_pMaterialInstance->m_PSShaderCustomValue[j]->m_ValueType == ValueType::VT_1)
				{
					ImGui::DragFloat(("##" + m_MaterialInstanceVirtualPath + m_pMaterialInstance->m_PSShaderCustomValue[j]->ConstValueName).c_str(), m_pMaterialInstance->m_PSShaderCustomValue[j]->m_Values.data(), 0.05f, 0.0f, 1.0f, "0.3f", ImGuiSliderFlags_None);
				}
				else if (m_pMaterialInstance->m_PSShaderCustomValue[j]->m_ValueType == ValueType::VT_2)
				{
					ImGui::DragFloat2(("##" + m_MaterialInstanceVirtualPath + m_pMaterialInstance->m_PSShaderCustomValue[j]->ConstValueName).c_str(), m_pMaterialInstance->m_PSShaderCustomValue[j]->m_Values.data(), 0.05f, 0.0f, 1.0f);
				}
				else if (m_pMaterialInstance->m_PSShaderCustomValue[j]->m_ValueType == ValueType::VT_3)
				{
					ImGui::DragFloat3(("##" + m_MaterialInstanceVirtualPath + m_pMaterialInstance->m_PSShaderCustomValue[j]->ConstValueName).c_str(), m_pMaterialInstance->m_PSShaderCustomValue[j]->m_Values.data(), 0.05f, 0.0f, 1.0f);
				}
				else if (m_pMaterialInstance->m_PSShaderCustomValue[j]->m_ValueType == ValueType::VT_4)
				{
					ImGui::DragFloat4(("##" + m_MaterialInstanceVirtualPath + m_pMaterialInstance->m_PSShaderCustomValue[j]->ConstValueName).c_str(), m_pMaterialInstance->m_PSShaderCustomValue[j]->m_Values.data(), 0.05f, 0.0f, 1.0f);
				}
			}

			std::map<std::string, std::string>& AssetRegistry = AssetManager::GetSingleton().GetTextureAssetRegistry();
			for (size_t j = 0; j < m_pMaterialInstance->m_PSShaderCustomTexture.size(); ++j)
			{
				ImGui::Text(m_pMaterialInstance->m_PSShaderCustomTexture[j]->ConstValueName.c_str());//variable name
				ImGui::SameLine();
				std::string temp = "##" + m_MaterialInstanceVirtualPath + m_pMaterialInstance->m_PSShaderCustomTexture[j]->ConstValueName;//temp widget label
				if (ImGui::BeginCombo(temp.c_str(), m_pMaterialInstance->m_PSShaderCustomTexture[j]->m_VirtualPath.c_str()))
				{
					for (auto& item : AssetRegistry)
					{
						bool isSelected = item.first == m_pMaterialInstance->m_PSShaderCustomTexture[j]->m_VirtualPath;
						if (ImGui::Selectable(item.first.c_str(), isSelected))
						{
							//component.m_MaterialPaths[i] = item.first;//need to load the sub material
							//component.m_Materials[i] = AssetManager::GetSingleton().GetMaterialInstance(component.m_MaterialPaths[i]);
							m_pMaterialInstance->m_PSShaderCustomTexture[j]->m_VirtualPath = item.first;
							m_pMaterialInstance->m_PSShaderCustomTexture[j]->m_pTexture = AssetManager::GetSingleton().GetTexture(item.first);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
					ImGui::SameLine();
				}
			}

			if (ImGui::Button("Save MaterialInstance"))
			{
				//save material instance
				rapidjson::StringBuffer strBuf;
				rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
				writer.StartObject();
				rttr::type materialType = rttr::type::get<MaterialInstance>();
				writer.Key(materialType.get_name().to_string().c_str());
				SceneSerializer::ToJsonRecursive(*m_pMaterialInstance, writer, true);
				writer.EndObject();
				std::string data = strBuf.GetString();
				//write to physical path
				std::ofstream fout("assets/" + AssetManager::GetSingleton().GetAssetPhysicalPath(m_MaterialInstanceVirtualPath));
				//std::ofstream fout(AssetManager::GetSingleton().Get);
				fout << data.c_str();
				fout.close();

				//notify asset manager to update material instance?

			}		
		}

		ImGui::End();
	}
}