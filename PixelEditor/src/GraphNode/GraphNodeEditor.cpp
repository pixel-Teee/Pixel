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

		//if is first open, then load the settings file
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

			m_Id = std::max(m_Id, (uint32_t)pGraphNode->m_NodeId.Get());
		}

		//in terms of the links to link the input node and put node
		for (auto& item : m_pMaterial->GetLinks())
		{
			uint32_t inputNodeId = item.first;
			uint32_t outputNodeId = item.second;

			Ref<GraphPin> inputPin;
			Ref<GraphPin> outputPin;

			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{
				if (m_GraphPins[i]->m_PinId.Get() == inputNodeId)
					inputPin = m_GraphPins[i];
				if (m_GraphPins[i]->m_PinId.Get() == outputNodeId)
					outputPin = m_GraphPins[i];
			}

			if (inputPin == nullptr)
			{
				inputPin = CreateRef<GraphPin>();
				inputPin->m_PinId = inputNodeId;
			}

			if (outputPin == nullptr)
			{
				outputPin = CreateRef<GraphPin>();
				outputPin->m_PinId = outputNodeId;
			}

			m_Id = std::max(m_Id, inputNodeId);
			m_Id = std::max(m_Id, outputNodeId);
		}

		//get the logic pin node
		std::vector<Ref<InputNode>> inputNodes;
		std::vector<Ref<OutputNode>> outputNodes;
		for (uint32_t i = 0; i < m_pMaterial->m_pShaderFunctionArray.size(); ++i)
		{
			for (uint32_t j = 0; j < m_pMaterial->m_pShaderFunctionArray[i]->GetInputNodeNum(); ++j)
				inputNodes.push_back(m_pMaterial->m_pShaderFunctionArray[i]->GetInputNode(j));

			for (uint32_t j = 0; j < m_pMaterial->m_pShaderFunctionArray[i]->GetOutputNodeNum(); ++j)
				outputNodes.push_back(m_pMaterial->m_pShaderFunctionArray[i]->GetOutputNode(j));
		}

		for (uint32_t i = 0; i < m_pMaterial->m_pShaderMainFunction->GetInputNodeNum(); ++i)
		{
			inputNodes.push_back(m_pMaterial->m_pShaderMainFunction->GetInputNode(i));
		}

		//link the graph pin and graph node
		for (auto& item : m_pMaterial->GetLinks())
		{
			uint32_t inputNodeId = item.first;
			uint32_t outputNodeId = item.second;

			Ref<ShaderFunction> pInputNodeOwnerShaderFunction;
			Ref<ShaderFunction> pOutputNodeOwnerShaderFunction;

			for (size_t i = 0; i < inputNodes.size(); ++i)
			{
				if (inputNodes[i]->m_id == inputNodeId)
					pInputNodeOwnerShaderFunction = inputNodes[i]->GetOwner();
			}

			for (size_t i = 0; i < outputNodes.size(); ++i)
			{
				if (outputNodes[i]->m_id == outputNodeId)
					pOutputNodeOwnerShaderFunction = outputNodes[i]->GetOwner();
			}

			Ref<GraphNode> pInputPinOwner;
			Ref<GraphNode> pOutputPinOwner;

			for (size_t i = 0; i < m_GraphNodes.size(); ++i)
			{
				if (m_GraphNodes[i]->m_NodeId.Get() == pInputNodeOwnerShaderFunction->GetFunctioNodeId()) pInputPinOwner = m_GraphNodes[i];

				if (m_GraphNodes[i]->m_NodeId.Get() == pOutputNodeOwnerShaderFunction->GetFunctioNodeId()) pOutputPinOwner = m_GraphNodes[i];
			}

			Ref<GraphPin> inputPin;
			Ref<GraphPin> outputPin;

			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{
				if (m_GraphPins[i]->m_PinId.Get() == inputNodeId) inputPin = m_GraphPins[i];
				if (m_GraphPins[i]->m_PinId.Get() == outputNodeId) outputPin = m_GraphPins[i];
			}

			pInputPinOwner->m_InputPin.push_back(inputPin);
			pOutputPinOwner->m_OutputPin.push_back(outputPin);
			inputPin->m_OwnerNode = pInputPinOwner;
			outputPin->m_OwnerNode = pOutputPinOwner;
		}

		//create the graph link
		for (auto& item : m_pMaterial->GetLinks())
		{
			Ref<GraphLink> pGraphLink = CreateRef<GraphLink>();
			pGraphLink->m_LinkId = ++m_Id;
			
			uint32_t inputNodeId = item.first;
			uint32_t outputNodeId = item.second;

			Ref<GraphPin> inputPin;
			Ref<GraphPin> outputPin;

			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{
				if (m_GraphPins[i]->m_PinId.Get() == inputNodeId) inputPin = m_GraphPins[i];
				if (m_GraphPins[i]->m_PinId.Get() == outputNodeId) outputPin = m_GraphPins[i];
			}

			pGraphLink->m_InputPin = inputPin;
			pGraphLink->m_OutputPin = outputPin;
			inputPin->m_NodeLink = pGraphLink;
			outputPin->m_NodeLink = pGraphLink;
			m_GraphLinks.push_back(pGraphLink);
		}
	}

	GraphNodeEditor::~GraphNodeEditor()
	{
	}

	void GraphNodeEditor::OnImGuiRender(bool& OpenGraphNodeEditor)
	{
		//when open, will from the material asset open, then load the grah editor json file
		
		ImGui::Begin("Graph Node Editor", &OpenGraphNodeEditor);

		//render
		ed::SetCurrentEditor(m_Editor);
			ed::Begin("Graph Node Editor Canvas");		
			//draw shader main function

			//TODO:in the future, will in terms of the shading model to swith this
			DrawPbrNode();

			ed::End();
		ed::SetCurrentEditor(nullptr);


		ImGui::End();
	}

	void GraphNodeEditor::DrawPbrNode()
	{
		
	}

}