#include "pxpch.h"

#include "GraphNodeEditor.h"

#include "GraphPin.h"
#include "GraphLink.h"
#include "GraphNode.h"
#include "Pixel/Renderer/3D/Material/Material.h"
#include "Pixel/Renderer/3D/Material/ShaderFunction.h"
#include "Pixel/Renderer/3D/Material/ShaderMainFunction.h"
#include "Pixel/Renderer/3D/Material/InputNode.h"
#include "Pixel/Renderer/3D/Material/OutputNode.h"

//------other library------
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
//------other library------

namespace Pixel {

	GraphNodeEditor::GraphNodeEditor(const std::string& virtualPath, Ref<Material> pMaterial)
	{
		m_Id = 0;

		//if it is first open, then load the settings file
		ed::Config config;

		m_GraphNodeEditorConfigPath = virtualPath + ".json";

		config.SettingsFile = m_GraphNodeEditorConfigPath.c_str();

		m_Editor = ed::CreateEditor(&config);

		//from the material to construct node¡¢pin¡¢link
		m_pMaterial = pMaterial;

		//create the graph node
		for (size_t i = 0; i < m_pMaterial->GetShaderFunction().size(); ++i)
		{
			Ref<GraphNode> pGraphNode;
			pGraphNode = CreateRef<GraphNode>();
			pGraphNode->m_NodeId = m_pMaterial->GetShaderFunction()[i]->GetFunctioNodeId();
			pGraphNode->p_Owner = m_pMaterial->GetShaderFunction()[i];

			m_Id = std::max(m_Id, (uint32_t)pGraphNode->m_NodeId.Get());

			for (size_t j = 0; j < m_pMaterial->GetShaderFunction()[i]->GetInputNodeNum(); ++j)
			{
				Ref<GraphPin> pGraphPins = CreateRef<GraphPin>();
				pGraphPins->m_PinId = m_pMaterial->GetShaderFunction()[i]->GetInputNode(j)->GetPutNodeId();
				pGraphPins->m_OwnerNode = pGraphNode;
				pGraphNode->m_InputPin.push_back(pGraphPins);
				m_GraphPins.push_back(pGraphPins);

				m_Id = std::max(m_Id, (uint32_t)pGraphPins->m_PinId.Get());
			}

			for (size_t j = 0; j < m_pMaterial->GetShaderFunction()[i]->GetOutputNodeNum(); ++j)
			{
				Ref<GraphPin> pGraphPins = CreateRef<GraphPin>();
				pGraphPins->m_PinId = m_pMaterial->GetShaderFunction()[i]->GetOutputNode(j)->GetPutNodeId();
				pGraphPins->m_OwnerNode = pGraphNode;
				pGraphNode->m_OutputPin.push_back(pGraphPins);
				m_GraphPins.push_back(pGraphPins);

				m_Id = std::max(m_Id, (uint32_t)pGraphPins->m_PinId.Get());
			}

			m_GraphNodes.push_back(pGraphNode);
		}

		for (auto& item : m_pMaterial->GetLinks())
		{
			uint32_t inputPinId = item.first;
			uint32_t outputPinId = item.second;

			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{
				Ref<GraphLink> pGraphLinks = CreateRef<GraphLink>();
				if (m_GraphPins[i]->m_PinId.Get() == inputPinId)
				{
					pGraphLinks->m_InputPin = m_GraphPins[i];
				}
				if (m_GraphPins[i]->m_PinId.Get() == outputPinId)
				{
					pGraphLinks->m_OutputPin = m_GraphPins[i];
				}
				++m_Id;
				pGraphLinks->m_LinkId = m_Id;
			}
		}
	}

	GraphNodeEditor::~GraphNodeEditor()
	{
	}

	void GraphNodeEditor::OnImGuiRender(bool& OpenGraphNodeEditor)
	{
		//when open, will from the material asset open, then load the graph editor json file
		
		ImGui::Begin("Graph Node Editor", &OpenGraphNodeEditor);

		//render
		ed::SetCurrentEditor(m_Editor);
			ed::Begin("Graph Node Editor Canvas");
			//draw shader main function

			//TODO:in the future, will in terms of the shading model to switch this
			DrawMainFunctionNode();

			//1.draw node

			//2.draw link

			//3.handle create node

			ed::End();
		ed::SetCurrentEditor(nullptr);


		ImGui::End();
	}

	void GraphNodeEditor::DrawMainFunctionNode()
	{
		ed::BeginNode(1);
		//for (size_t i = 0; i < m_pMaterial->GetMainFunction()->GetInputNodeNum(); ++i)
		//{
		//	ed::BeginPin(m_pMaterial->GetMainFunction()->GetInputNode(i)->GetPutNodeId(), ed::PinKind::Input);
		//	ImGui::Text(m_pMaterial->GetMainFunction()->GetInputNode(i)->GetNodeName().c_str());
		//	ed::EndPin();
		//}
		for (size_t i = 0; i < m_GraphNodes.size(); ++i)
		{
			if (m_GraphNodes[i]->m_NodeId.Get() == 1)
			{
				for (size_t j = 0; j < m_GraphNodes[i]->p_Owner->GetInputNodeNum(); ++j)
				{
					ed::BeginPin(m_GraphNodes[i]->p_Owner->GetInputNode(j)->m_id, ed::PinKind::Input);
					ImGui::BeginHorizontal(m_GraphNodes[i]->p_Owner->GetInputNode(j)->m_id);
					DrawPinIcon(ImColor(255.0f, 255.0f, 255.0f, 255.0f), ImColor(32.0f, 32.0f, 32.0f, 255.0f));
					ImGui::Text(m_GraphNodes[i]->p_Owner->GetInputNode(j)->GetNodeName().c_str());
					//Draw Pin Icon
					ImGui::EndHorizontal();
					ed::EndPin();
				}
			}
		}
		ed::EndNode();
	}

	void GraphNodeEditor::DrawLinks()
	{
		//draw links
		for (auto& linkInfo : m_GraphLinks)
			ed::Link(linkInfo->m_LinkId, linkInfo->m_InputPin.lock()->m_PinId, linkInfo->m_OutputPin.lock()->m_PinId);
	}

	void GraphNodeEditor::DrawPinIcon(ImU32 color, ImU32 innerColor)
	{
		float PinIconSize = 24.0f;

		auto cursorPos = ImGui::GetCursorScreenPos();
		auto drawList = ImGui::GetWindowDrawList();
		
		//cursorPos
		//cursorPos + size

		auto rect = ImRect(cursorPos, cursorPos + ImVec2(PinIconSize, PinIconSize));
		auto rect_x = rect.Min.x;
		auto rect_y = rect.Min.y;
		auto rect_w = rect.Max.x - rect.Min.x;
		auto rect_h = rect.Max.y - rect.Min.y;
		auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
		auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
		auto rect_center = ImVec2(rect_center_x, rect_center_y);

		const auto outlineScale = rect_w / 24.0f;
		const auto extraSegments = static_cast<int>(2 * outlineScale);

		//draw circle and triangle
		auto triangleStart = rect_center_x + 0.32f * rect_w;
		auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

		rect.Min.x += rect_offset;
		rect.Max.x += rect_offset;
		rect_x += rect_offset;
		rect_center_x += rect_offset * 0.5f;
		rect_center_x += rect_offset * 0.5f;

		//draw circle
		const auto c = rect_center;
		
		const auto r = 0.5f * rect_w / 2.0f - 0.5f;

		drawList->AddCircleFilled(c, r, innerColor, 12 + extraSegments);
		drawList->AddCircle(c, r, color, 12 + extraSegments, 2.0f * outlineScale);

		//draw triangle
		const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

		drawList->AddTriangleFilled(
			ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
			ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
			ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
			color);

		ImGui::Dummy(ImVec2(PinIconSize, PinIconSize));
	}

}