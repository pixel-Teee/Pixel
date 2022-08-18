#include "pxpch.h"

#include "NodeGraph.h"

namespace Pixel {

	NodeGraph::NodeGraph()
	{
		//m_previewBox = Model("assets/models/Box.obj");
		//
		//m_pCamera = CreateRef<Camera>();
		//
		////TODO:improve the staticmesh class's architecture
		//m_pStaticMesh = CreateRef<StaticMesh>(*m_previewBox.GetMeshes()[0]);
		//
		//m_pMaterial = CreateRef<Material>("Test", Material::MUT_GEO);
		//m_pMaterialInstance = CreateRef<MaterialInstance>(m_pMaterial);
		//
		//m_materialPass.SetCamera(m_pCamera);
		//m_materialPass.SetMaterialInstance(m_pMaterialInstance);
		//m_materialPass.SetMesh(m_pStaticMesh);
		//
		////------Node Graph------
		//m_isopen = false;
		//m_isFirstFrame = true;
		//m_uniqueId = 0;
		//
		//ed::Config config;
		//config.SettingsFile = "BasicInteraction.json";
		//g_Context = ed::CreateEditor(&config);
		//ed::SetCurrentEditor(g_Context);
		////Create Pbr Node
		//m_pbrNode = CreateRef<GraphNode>();
		//m_pbrNode->p_Owner = m_pMaterial->GetMainFunction(0);
		//m_pbrNode->m_NodeId = ++m_uniqueId;
		//for (uint32_t i = 0; i < 6; ++i)
		//{
		//	CreateNodePin(m_pbrNode, i);
		//}
		////------Node Graph------
	}

	NodeGraph::NodeGraph(Ref<Material> pMaterial, Ref<MaterialInstance> pMaterialInstance, std::string filePath)
	{
		//reverse the material tree to init
		//m_isopen = false;
		//m_isFirstFrame = true;
		//m_uniqueId = 0;
		//m_pMaterial = pMaterial;
		//m_pMaterialInstance = pMaterialInstance;
		//
		//ed::Config config;
		//config.SettingsFile = "BasicInteraction.json";
		//g_Context = ed::CreateEditor(&config);
		//ed::SetCurrentEditor(g_Context);
		//
		////---need to refractor------
		//for (uint32_t i = 0; i < pMaterial->GetShaderFunction().size(); ++i)
		//{
		//	Ref<ShaderFunction> pShaderFunction = pMaterial->GetShaderFunction()[i];
		//	switch (pShaderFunction->GetFunctionType())
		//	{
		//	case ShaderFunction::ShaderFunctionType::ConstFloat4:
		//		{
		//			Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		//			pGraphNode->m_NodeId = ++m_uniqueId;
		//			pGraphNode->p_Owner = pShaderFunction;
		//
		//			if (pShaderFunction->IsHaveInput())
		//			{
		//				pGraphNode->m_InputPin.resize(pShaderFunction->GetInputNodeNum());
		//				for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
		//				{
		//					pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
		//					pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//					pGraphNode->m_InputPin[i]->m_LocationIndex = i;
		//					pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
		//				}
		//			}
		//
		//			if (pShaderFunction->IsHaveOutput())
		//			{
		//				pGraphNode->m_OutputPin.resize(pShaderFunction->GetOutputNodeNum());
		//				for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
		//				{
		//					pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//					pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//					pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
		//					pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
		//				}
		//			}
		//			m_Nodes.push_back(pGraphNode);
		//			//set node pos
		//			ed::SetNodePosition(pGraphNode->m_NodeId, ImVec2(pShaderFunction->GetFunctioNodePos().x,
		//				pShaderFunction->GetFunctioNodePos().y));
		//			break;
		//		}
		//	case ShaderFunction::ShaderFunctionType::Mul:
		//	{
		//		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		//		pGraphNode->m_NodeId = ++m_uniqueId;
		//		pGraphNode->p_Owner = pShaderFunction;
		//
		//		if (pShaderFunction->IsHaveInput())
		//		{
		//			pGraphNode->m_InputPin.resize(pShaderFunction->GetInputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
		//			{
		//				pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_InputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//
		//		if (pShaderFunction->IsHaveOutput())
		//		{
		//			pGraphNode->m_OutputPin.resize(pShaderFunction->GetOutputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
		//			{
		//				pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//		m_Nodes.push_back(pGraphNode);
		//		//set node pos
		//		ed::SetNodePosition(pGraphNode->m_NodeId, ImVec2(pShaderFunction->GetFunctioNodePos().x,
		//			pShaderFunction->GetFunctioNodePos().y));
		//		break;
		//	}
		//	case ShaderFunction::ShaderFunctionType::Sampler2D:
		//	{
		//		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		//		pGraphNode->m_NodeId = ++m_uniqueId;
		//		pGraphNode->p_Owner = pShaderFunction;
		//
		//		if (pShaderFunction->IsHaveInput())
		//		{
		//			pGraphNode->m_InputPin.resize(pShaderFunction->GetInputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
		//			{
		//				pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_InputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//
		//		if (pShaderFunction->IsHaveOutput())
		//		{
		//			pGraphNode->m_OutputPin.resize(pShaderFunction->GetOutputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
		//			{
		//				pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//		m_Nodes.push_back(pGraphNode);
		//		//set node pos
		//		ed::SetNodePosition(pGraphNode->m_NodeId, ImVec2(pShaderFunction->GetFunctioNodePos().x,
		//		pShaderFunction->GetFunctioNodePos().y));
		//		break;
		//	}
		//	}
		//}
		//
		////Create Pin
		//
		//for (uint32_t i = 0; i < pMaterial->GetShaderFunction().size(); ++i)
		//{
		//	for (uint32_t j = 0; j < pMaterial->GetShaderFunction()[i]->GetInputNodeNum(); ++j)
		//	{
		//		Ref<InputNode> pInputNode = pMaterial->GetShaderFunction()[i]->GetInputNode(j);
		//		
		//		Ref<OutputNode> pOutputNode = pInputNode->GetOutputLink();
		//		if (pOutputNode == nullptr)
		//		{
		//			continue;
		//		}
		//		Ref<ShaderFunction> pOutputShaderFunction = pOutputNode->GetOwner();
		//		uint32_t OutputLocation = -1;
		//		//find location
		//		for (uint32_t k = 0; k < pOutputShaderFunction->GetOutputNodeNum(); ++k)
		//		{
		//			if (pOutputShaderFunction->GetOutputNode(k) == pOutputNode)
		//			{
		//				OutputLocation = k;
		//			}
		//		}
		//		Ref<GraphNodeLink> pLink = CreateRef<GraphNodeLink>();
		//		//find the pInputNode and pOutputNode's graph node
		//		Ref<GraphNode> pInputGraphNode;
		//		Ref<GraphNode> pOutputGraphNode;
		//		for (auto node : m_Nodes)
		//		{
		//			if (node->p_Owner == pInputNode->GetOwner()) pInputGraphNode = node;
		//			if (node->p_Owner == pOutputNode->GetOwner()) pOutputGraphNode = node;
		//		}
		//
		//		if (pInputGraphNode != nullptr && pOutputGraphNode != nullptr)
		//		{
		//			pLink->m_LinkId = ++m_uniqueId;
		//			pLink->m_InputPin = pInputGraphNode->m_InputPin[j];
		//			PX_CORE_ASSERT(OutputLocation != -1, "Error");
		//			pLink->m_OutputPin = pOutputGraphNode->m_OutputPin[OutputLocation];
		//			m_NodeLink.push_back(pLink);
		//		}
		//	}
		//}
		//
		//m_pbrNode = CreateRef<GraphNode>();
		//m_pbrNode->m_NodeId = ++m_uniqueId;
		//m_pbrNode->p_Owner = pMaterial->GetMainFunction(0);
		//
		//for (uint32_t i = 0; i < 6; ++i)
		//{
		//	CreateNodePin(m_pbrNode, i);
		//}	
		//
		//m_filePath = filePath;
	}

	NodeGraph::~NodeGraph()
	{
		//ed::DestroyEditor(g_Context);
	}

	void NodeGraph::OnImGuiRender()
	{
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//ImGui::Begin("Graph Editor Context");
		//
		//auto [mx, my] = ImGui::GetMousePos();
		//m_MousePos = { mx, my };
		//
		//ed::Begin("Graph Editor", ImVec2(0.0, 0.0f));
		//
		//if (ImGui::Button("Save"))
		//{
		//	for (auto node : m_Nodes)
		//	{
		//		Ref<ShaderFunction> pShaderFunction = node->p_Owner;
		//		ImVec2 pos = ed::GetNodePosition(node->m_NodeId);
		//		//PIXEL_CORE_INFO("{0}, {1}", pos.x, pos.y);
		//		pShaderFunction->SetFunctionNodePos(glm::vec2(pos.x, pos.y));
		//	}
		//	//m_materialPass.Draw();
		//	//SerializerMaterial seralizer;
		//	//seralizer.SerializerMaterialAsset(m_filePath, m_pMaterial, m_pMaterialInstance);
		//}
		//	
		//CreateNewNodeMenu();
		//
		////Draw PBR Node
		//DrawPbrNode();
		//
		////Draw Node
		//for (uint32_t i = 0; i < m_Nodes.size(); ++i)
		//{
		//	ed::BeginNode(m_Nodes[i]->m_NodeId);
		//	ImGui::Text((m_Nodes[i]->p_Owner->GetShowName()).c_str());
		//	if (m_Nodes[i]->p_Owner->IsHaveInput())
		//	{
		//		for (uint32_t j = 0; j < m_Nodes[i]->m_InputPin.size(); ++j)
		//		{
		//			ed::BeginPin(m_Nodes[i]->m_InputPin[j]->m_PinId, ed::PinKind::Input);
		//			ImGui::Text(("->" + m_Nodes[i]->p_Owner->GetInputNode(j)->GetNodeName()).c_str());
		//			ed::EndPin();
		//		}
		//	}
		//	if (m_Nodes[i]->p_Owner->IsHaveOutput())
		//	{
		//		for (uint32_t j = 0; j < m_Nodes[i]->m_OutputPin.size(); ++j)
		//		{
		//			ed::BeginPin(m_Nodes[i]->m_OutputPin[j]->m_PinId, ed::PinKind::Output);
		//			ImGui::Text((m_Nodes[i]->p_Owner->GetOutputNode(j)->GetNodeName() + "->").c_str());
		//			ed::EndPin();
		//		}
		//	}
		//	ed::EndNode();
		//}
		//
		////draw link
		//for (auto& links : m_NodeLink)
		//	ed::Link(links->m_LinkId, links->m_InputPin->m_PinId, links->m_OutputPin->m_PinId);
		//
		////Handle Create link or node
		//if (ed::BeginCreate())
		//{
		//	ed::PinId inputPinId, outputPinId;
		//	if (ed::QueryNewLink(&inputPinId, &outputPinId))
		//	{
		//		if (inputPinId && outputPinId)
		//		{
		//			if (ed::AcceptNewItem())
		//			{
		//				//pin's owner node
		//				Ref<GraphNode> inPinNode;
		//				Ref<GraphNode> outPinNode;
		//
		//				Ref<GraphPin> inPin;
		//				Ref<GraphPin> outPin;
		//
		//				for (uint32_t i = 0; i < m_pbrNode->m_InputPin.size(); ++i)
		//				{
		//					if (m_pbrNode->m_InputPin[i]->m_PinId == outputPinId)
		//					{
		//						inPinNode = m_pbrNode;
		//						inPin = m_pbrNode->m_InputPin[i];
		//					}
		//				}
		//
		//				//find pin's owner
		//				for (uint32_t i = 0; i < m_Nodes.size(); ++i)
		//				{
		//					bool flag = false;
		//					for (uint32_t j = 0; j < m_Nodes[i]->m_InputPin.size(); ++j)
		//					{
		//						if (m_Nodes[i]->m_InputPin[j] != nullptr && m_Nodes[i]->m_InputPin[j]->m_PinId == outputPinId)
		//						{
		//							flag = true;
		//							inPinNode = m_Nodes[i];
		//							inPin = m_Nodes[i]->m_InputPin[j];
		//							break;
		//						}
		//					}
		//					if (flag) break;
		//				}
		//				for (uint32_t i = 0; i < m_Nodes.size(); ++i)
		//				{
		//					bool flag = false;
		//					for (uint32_t j = 0; j < m_Nodes[i]->m_OutputPin.size(); ++j)
		//					{
		//						if (m_Nodes[i]->m_OutputPin[j] != nullptr && m_Nodes[i]->m_OutputPin[j]->m_PinId == inputPinId)
		//						{
		//							flag = true;
		//							outPinNode = m_Nodes[i];
		//							outPin = m_Nodes[i]->m_OutputPin[j];
		//							break;
		//						}
		//					}
		//					if (flag) break;
		//				}
		//
		//				//create link
		//				Ref<GraphNodeLink> pLink = CreateRef<GraphNodeLink>();
		//				pLink->m_InputPin = inPin;
		//				pLink->m_OutputPin = outPin;
		//				pLink->m_LinkId = ++m_uniqueId;
		//
		//				//logic node link
		//				Ref<InputNode> logicInputNode = inPinNode->p_Owner->GetInputNode(inPin->m_LocationIndex);
		//				Ref<OutputNode> logicOutputNode = outPinNode->p_Owner->GetOutputNode(outPin->m_LocationIndex);
		//				logicInputNode->Connection(logicOutputNode);
		//
		//				m_NodeLink.push_back(pLink);
		//
		//				ed::Link(m_NodeLink.back()->m_LinkId, inPin->m_PinId, outPin->m_PinId);
		//			}
		//		}
		//	}
		//}
		//ed::EndCreate();
		//
		//if (ed::BeginDelete())
		//{
		//	ed::LinkId deletedLinkId;
		//	while (ed::QueryDeletedLink(&deletedLinkId))
		//	{
		//		for (auto& link : m_NodeLink)
		//		{
		//			if (link->m_LinkId == deletedLinkId)
		//			{
		//				Ref<GraphPin> pInputPin = link->m_InputPin;
		//				Ref<GraphPin> pOutputPin = link->m_OutputPin;
		//
		//				pInputPin->m_NodeLink = nullptr;
		//				pOutputPin->m_NodeLink = nullptr;
		//
		//				Ref<GraphNode> m_pInputNode = pInputPin->m_Node;
		//				Ref<GraphNode> m_pOutputNode = pOutputPin->m_Node;
		//
		//				//logic node
		//				Ref<ShaderFunction> m_pInputFunction = m_pInputNode->p_Owner;
		//				Ref<ShaderFunction> m_pOutputFunction = m_pOutputNode->p_Owner;
		//
		//				Ref<InputNode> pInputNode = m_pInputFunction->GetInputNode(pInputPin->m_LocationIndex);
		//				Ref<OutputNode> pOutputNode = m_pOutputFunction->GetOutputNode(pOutputPin->m_LocationIndex);
		//
		//				if (pInputNode->GetOutputLink() == pOutputNode)
		//				{
		//					pInputNode->DisConnection();
		//				}
		//
		//				auto it = find(m_NodeLink.begin(), m_NodeLink.end(), link);
		//				if (it != m_NodeLink.end())
		//				{
		//					m_NodeLink.erase(it);
		//				}
		//				break;
		//			}
		//		}
		//	}
		//}
		//ed::EndDelete();
		//ed::End();
		//ImGui::End();
		//ImGui::PopStyleVar();
	}

	void NodeGraph::DrawPbrNode()
	{
		//Ref<ShaderMainFunction> mainFunction = m_pMaterialInstance->GetMaterial()->GetMainFunction(0);
		//
		//ed::BeginNode(m_pbrNode->m_NodeId);
		//	ImGui::Text(mainFunction->GetShowName().c_str());
		//	ed::BeginPin(m_pbrNode->m_InputPin[0]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetWorldPosNode()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	//Draw Albedo Pin
		//	ed::BeginPin(m_pbrNode->m_InputPin[1]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetAlbedoNode()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	ed::BeginPin(m_pbrNode->m_InputPin[2]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetNormalNode()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	ed::BeginPin(m_pbrNode->m_InputPin[3]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetRoughness()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	ed::BeginPin(m_pbrNode->m_InputPin[4]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetMetallicNode()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	ed::BeginPin(m_pbrNode->m_InputPin[5]->m_PinId, ed::PinKind::Input);
		//		ImGui::Text(("->" + mainFunction->GetEmissiveNode()->GetNodeName()).c_str());
		//	ed::EndPin();
		//	//ImGui::Text()
		//ed::EndNode();
	}

	bool NodeGraph::CreateNewNodeMenu()
	{
		//ed::Suspend();
		//if (ed::ShowBackgroundContextMenu())
		//	ImGui::OpenPopup("CreateNewNode", ImGuiPopupFlags_MouseButtonRight);
		//if (ImGui::BeginPopup("CreateNewNode"))
		//{
		//	if (ImGui::MenuItem("ConstColor4"))
		//	{
		//		++m_uniqueId;
		//		//Create a ConstFloatValue Node, and add to m_pMaterial shaderfunction
		//		Ref<ConstFloatValue> floatValue4 = CreateRef<ConstFloatValue>(std::string("ConstColor4"), m_pMaterial, 4, false);
		//		//two phase initializer
		//		floatValue4->ConstrcutPutNodeAndSetPutNodeOwner();
		//		floatValue4->AddToMaterialOwner();
		//
		//		//Create a Graph Node
		//		Ref<GraphNode> floatNode = CreateRef<GraphNode>();
		//		floatNode->m_NodeId = m_uniqueId;
		//		floatNode->p_Owner = floatValue4;
		//		
		//		//in terms of floatValue4 to create pin
		//		if (floatValue4->IsHaveInput())
		//		{
		//			floatNode->m_InputPin.reserve(floatValue4->GetInputNodeNum());
		//			for (uint32_t i = 0; i < floatValue4->GetOutputNodeNum(); ++i)
		//			{
		//				floatNode->m_InputPin[i] = CreateRef<GraphPin>();
		//				floatNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//				floatNode->m_InputPin[i]->m_LocationIndex = i;
		//				floatNode->m_InputPin[i]->m_Node = floatNode;
		//			}
		//		}
		//		if (floatValue4->IsHaveOutput())
		//		{
		//			floatNode->m_OutputPin.resize(floatValue4->GetOutputNodeNum());
		//			for (uint32_t i = 0; i < floatValue4->GetOutputNodeNum(); ++i)
		//			{
		//				floatNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//				floatNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//				floatNode->m_OutputPin[i]->m_LocationIndex = i;
		//				floatNode->m_OutputPin[i]->m_Node = floatNode;
		//			}
		//		}
		//		m_Nodes.push_back(floatNode);
		//		ImVec2 pos = ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y));
		//		PIXEL_CORE_INFO("{0}, {1}", pos.x, pos.y);
		//		ed::SetNodePosition(floatNode->m_NodeId, ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y)));
		//		ImVec2 newpos = ed::GetNodePosition(floatNode->m_NodeId);
		//		//get pos
		//		PIXEL_CORE_INFO("{0}, {1}", newpos.x, newpos.y);
		//	}
		//
		//	if (ImGui::MenuItem("Mul"))
		//	{
		//		++m_uniqueId;
		//		Ref<Mul> pMul = CreateRef<Mul>(std::string("Mul"), m_pMaterial);
		//		pMul->ConstrcutPutNodeAndSetPutNodeOwner();
		//		pMul->AddToMaterialOwner();
		//
		//		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		//		pGraphNode->m_NodeId = m_uniqueId;
		//		pGraphNode->p_Owner = pMul;
		//
		//		if (pMul->IsHaveInput())
		//		{
		//			pGraphNode->m_InputPin.resize(pMul->GetInputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
		//			{
		//				pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_InputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//
		//		if (pMul->IsHaveOutput())
		//		{
		//			pGraphNode->m_OutputPin.resize(pMul->GetOutputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
		//			{
		//				pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//		m_Nodes.push_back(pGraphNode);
		//		ImVec2 pos = ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y));
		//		PIXEL_CORE_INFO("{0}, {1}", pos.x, pos.y);
		//		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y)));
		//		ImVec2 newpos = ed::GetNodePosition(pGraphNode->m_NodeId);
		//		//get pos
		//		PIXEL_CORE_INFO("{0}, {1}", newpos.x, newpos.y);
		//	}
		//
		//	if (ImGui::MenuItem("Texture2D"))
		//	{
		//		++m_uniqueId;
		//		Ref<TexSampler> pTexture = CreateRef<TexSampler>(std::string("TexSampler"), m_pMaterial);
		//		pTexture->ConstrcutPutNodeAndSetPutNodeOwner();
		//		pTexture->AddToMaterialOwner();
		//
		//		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		//		pGraphNode->m_NodeId = m_uniqueId;
		//		pGraphNode->p_Owner = pTexture;
		//
		//		if (pTexture->IsHaveInput())
		//		{
		//			pGraphNode->m_InputPin.resize(pTexture->GetInputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
		//			{
		//				pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_InputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//
		//		if (pTexture->IsHaveOutput())
		//		{
		//			pGraphNode->m_OutputPin.resize(pTexture->GetOutputNodeNum());
		//			for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
		//			{
		//				pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
		//				pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
		//				pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
		//				pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
		//			}
		//		}
		//		m_Nodes.push_back(pGraphNode);
		//		ImVec2 pos = ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y));
		//		PIXEL_CORE_INFO("{0}, {1}", pos.x, pos.y);
		//		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y)));
		//		ImVec2 newpos = ed::GetNodePosition(pGraphNode->m_NodeId);
		//		//get pos
		//		PIXEL_CORE_INFO("{0}, {1}", newpos.x, newpos.y);
		//	}
		//	ImGui::EndPopup();
		//}
		//
		//ed::Resume();
		//
		//return true;
	}

	void NodeGraph::CreateNodePin(Ref<GraphNode> graphNode, uint32_t locationId)
	{
		//graphNode->m_InputPin.push_back(CreateRef<GraphPin>());
		//graphNode->m_InputPin.back()->m_PinId = ++m_uniqueId;
		//graphNode->m_InputPin.back()->m_LocationIndex = locationId;
		//graphNode->m_InputPin.back()->m_Node = graphNode;
	}
}