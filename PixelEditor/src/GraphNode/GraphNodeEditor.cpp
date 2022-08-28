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

		for (size_t i = 0; i < m_pMaterial->GetShaderFunction().size(); ++i)
		{
			Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
			pGraphNode->m_NodeId = m_pMaterial->GetShaderFunction()[i]->GetFunctioNodeId();
			pGraphNode->p_Owner = m_pMaterial->GetShaderFunction()[i];
			m_GraphNodes.push_back(pGraphNode);

			m_Id = std::max(m_Id, (uint32_t)pGraphNode->m_NodeId.Get());
		}

		for (auto& item : m_pMaterial->GetLinks())
		{
			m_Id = std::max(m_Id, std::max(item.first, item.second));
		}

		//in terms of the links to link the input node and output node
		for (auto& item : m_pMaterial->GetLinks())
		{
			uint32_t inputPinId = item.first;
			uint32_t outputPinId = item.second;

			Ref<GraphNode> inputPinOwner;
			Ref<GraphNode> outputPinOwner;

			for (size_t i = 0; i < m_GraphNodes.size(); ++i)
			{
				if (m_GraphNodes[i]->m_NodeId.Get() == inputPinId) inputPinOwner = m_GraphNodes[i];
				if (m_GraphNodes[i]->m_NodeId.Get() == outputPinId) outputPinOwner = m_GraphNodes[i];
			}

			Ref<GraphPin> inputPin;
			Ref<GraphPin> outputPin;
			//to easure uniqueness
			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{
				if (m_GraphPins[i]->m_PinId.Get() == inputPinId) inputPin = m_GraphPins[i];
				if (m_GraphPins[i]->m_PinId.Get() == outputPinId) outputPin = m_GraphPins[i];
			}

			if (inputPin == nullptr)
			{
				inputPin = CreateRef<GraphPin>();
				inputPin->m_PinId = inputPinId;
				inputPin->m_OwnerNode = inputPinOwner;
				inputPinOwner->m_InputPin.push_back(inputPin);
				m_GraphPins.push_back(inputPin);
			}

			if (outputPin == nullptr)
			{
				outputPin = CreateRef<GraphPin>();
				outputPin->m_PinId = outputPinId;
				outputPin->m_OwnerNode = outputPinOwner;
				outputPinOwner->m_OutputPin.push_back(outputPin);
				m_GraphPins.push_back(outputPin);
			}

			++m_Id;
			//creat the graph link
			Ref<GraphLink> graphLink = CreateRef<GraphLink>();
			graphLink->m_LinkId = m_Id;
			graphLink->m_InputPin = inputPin;
			graphLink->m_OutputPin = outputPin;
			m_GraphLinks.push_back(graphLink);
		}

		//m_MainShaderFunctionNodeId = pMaterial->GetMainFunction()->GetFunctioNodeId();
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
		ed::BeginNode(m_pMaterial->GetMainFunction()->GetFunctioNodeId());
		for (size_t i = 0; i < m_pMaterial->GetMainFunction()->GetInputNodeNum(); ++i)
		{
			ed::BeginPin(m_pMaterial->GetMainFunction()->GetInputNode(i)->GetPutNodeId(), ed::PinKind::Input);
			ImGui::Text(m_pMaterial->GetMainFunction()->GetInputNode(i)->GetNodeName().c_str());
			ed::EndPin();
		}
		ed::EndNode();
	}

	void GraphNodeEditor::DrawLinks()
	{
		//draw links
		for (auto& linkInfo : m_GraphLinks)
			ed::Link(linkInfo->m_LinkId, linkInfo->m_InputPin.lock()->m_PinId, linkInfo->m_OutputPin.lock()->m_PinId);
	}

}