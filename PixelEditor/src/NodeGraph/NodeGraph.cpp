#include "pxpch.h"
#include "NodeGraph.h"

#include "Pixel/Renderer/3D/Mul.h"

namespace Pixel {

	NodeGraph::NodeGraph()
	{
		m_previewBox = Model("assets/models/Box.obj");
		
		m_pCamera = CreateRef<Camera>();

		//TODO:improve the staticmesh class's architecture
		m_pStaticMesh = CreateRef<StaticMesh>(m_previewBox.GetMeshes()[0]);
		
		m_pMaterial = CreateRef<Material>("Test", Material::MUT_GEO);
		m_pMaterialInstance = CreateRef<MaterialInstance>(m_pMaterial);

		m_materialPass.SetCamera(m_pCamera);
		m_materialPass.SetMaterialInstance(m_pMaterialInstance);
		m_materialPass.SetMesh(m_pStaticMesh);

		//------Node Graph------
		m_isopen = false;
		m_isFirstFrame = true;
		m_uniqueId = 0;
		
		ed::Config config;
		config.SettingsFile = "BasicInteraction.json";
		g_Context = ed::CreateEditor(&config);
		ed::SetCurrentEditor(g_Context);
		//Create Pbr Node
		m_pbrNode = CreateRef<GraphNode>();
		m_pbrNode->p_Owner = m_pMaterial->GetMainFunction(0);
		m_pbrNode->m_NodeId = ++m_uniqueId;
		for (uint32_t i = 0; i < 6; ++i)
		{
			CreateNodePin(m_pbrNode, i);
		}
		//------Node Graph------
	}

	NodeGraph::~NodeGraph()
	{
		ed::DestroyEditor(g_Context);
	}

	void NodeGraph::OnImGuiRender()
	{
		//------Open Graph Editor------
		ImGui::Begin("Test Button");
		if (ImGui::Button("Test"))
		{
			if (m_isopen) m_isopen = false;
			else m_isopen = true;
		}
		ImGui::End();
		//------Open Graph Editor------

		if (m_isopen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Graph Editor Context");

			auto [mx, my] = ImGui::GetMousePos();
			m_MousePos = { mx, my };

			ed::Begin("Graph Editor", ImVec2(0.0, 0.0f));

			if (ImGui::Button("Compiler"))
			{
				m_materialPass.Draw();
			}
			
			CreateNewNodeMenu();

			//Draw PBR Node
			DrawPbrNode();

			//Draw Node
			for (uint32_t i = 0; i < m_Nodes.size(); ++i)
			{
				ed::BeginNode(m_Nodes[i]->m_NodeId);
				ImGui::Text((m_Nodes[i]->p_Owner->GetShowName()).c_str());
				if (m_Nodes[i]->p_Owner->IsHaveInput())
				{
					for (uint32_t j = 0; j < m_Nodes[i]->m_InputPin.size(); ++j)
					{
						ed::BeginPin(m_Nodes[i]->m_InputPin[j]->m_PinId, ed::PinKind::Input);
						ImGui::Text(("->" + m_Nodes[i]->p_Owner->GetInputNode(j)->GetNodeName()).c_str());
						ed::EndPin();
					}
				}
				if (m_Nodes[i]->p_Owner->IsHaveOutput())
				{
					for (uint32_t j = 0; j < m_Nodes[i]->m_OutputPin.size(); ++j)
					{
						ed::BeginPin(m_Nodes[i]->m_OutputPin[j]->m_PinId, ed::PinKind::Output);
						ImGui::Text((m_Nodes[i]->p_Owner->GetOutputNode(j)->GetNodeName() + "->").c_str());
						ed::EndPin();
					}
				}
				ed::EndNode();
			}

			//draw link
			for (auto& links : m_NodeLink)
				ed::Link(links->m_LinkId, links->m_InputPin->m_PinId, links->m_OutputPin->m_PinId);

			//Handle Create link or node
			if (ed::BeginCreate())
			{
				ed::PinId inputPinId, outputPinId;
				if (ed::QueryNewLink(&inputPinId, &outputPinId))
				{
					if (inputPinId && outputPinId)
					{
						if (ed::AcceptNewItem())
						{
							//pin's owner node
							Ref<GraphNode> inPinNode;
							Ref<GraphNode> outPinNode;

							Ref<GraphPin> inPin;
							Ref<GraphPin> outPin;

							for (uint32_t i = 0; i < m_pbrNode->m_InputPin.size(); ++i)
							{
								if (m_pbrNode->m_InputPin[i]->m_PinId == outputPinId)
								{
									inPinNode = m_pbrNode;
									inPin = m_pbrNode->m_InputPin[i];
								}
							}

							//find pin's owner
							for (uint32_t i = 0; i < m_Nodes.size(); ++i)
							{
								bool flag = false;
								for (uint32_t j = 0; j < m_Nodes[i]->m_InputPin.size(); ++j)
								{
									if (m_Nodes[i]->m_InputPin[j] != nullptr && m_Nodes[i]->m_InputPin[j]->m_PinId == outputPinId)
									{
										flag = true;
										inPinNode = m_Nodes[i];
										inPin = m_Nodes[i]->m_InputPin[j];
										break;
									}
								}
								if (flag) break;
							}
							for (uint32_t i = 0; i < m_Nodes.size(); ++i)
							{
								bool flag = false;
								for (uint32_t j = 0; j < m_Nodes[i]->m_OutputPin.size(); ++j)
								{
									if (m_Nodes[i]->m_OutputPin[j] != nullptr && m_Nodes[i]->m_OutputPin[j]->m_PinId == inputPinId)
									{
										flag = true;
										outPinNode = m_Nodes[i];
										outPin = m_Nodes[i]->m_OutputPin[j];
										break;
									}
								}
								if (flag) break;
							}

							//create link
							Ref<GraphNodeLink> pLink = CreateRef<GraphNodeLink>();
							pLink->m_InputPin = inPin;
							pLink->m_OutputPin = outPin;
							pLink->m_LinkId = ++m_uniqueId;

							//logic node link
							Ref<InputNode> logicInputNode = inPinNode->p_Owner->GetInputNode(inPin->m_LocationIndex);
							Ref<OutputNode> logicOutputNode = outPinNode->p_Owner->GetOutputNode(outPin->m_LocationIndex);
							logicInputNode->Connection(logicOutputNode);

							m_NodeLink.push_back(pLink);

							ed::Link(m_NodeLink.back()->m_LinkId, inPin->m_PinId, outPin->m_PinId);
						}
					}
				}
			}
			ed::EndCreate();
			ed::End();
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void NodeGraph::DrawPbrNode()
	{
		Ref<ShaderMainFunction> mainFunction = m_pMaterialInstance->GetMaterial()->GetMainFunction(0);
		
		ed::BeginNode(m_pbrNode->m_NodeId);
			ImGui::Text(mainFunction->GetShowName().c_str());
			ed::BeginPin(m_pbrNode->m_InputPin[0]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetWorldPosNode()->GetNodeName()).c_str());
			ed::EndPin();
			//Draw Albedo Pin
			ed::BeginPin(m_pbrNode->m_InputPin[1]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetAlbedoNode()->GetNodeName()).c_str());
			ed::EndPin();
			ed::BeginPin(m_pbrNode->m_InputPin[2]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetNormalNode()->GetNodeName()).c_str());
			ed::EndPin();
			ed::BeginPin(m_pbrNode->m_InputPin[3]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetRoughness()->GetNodeName()).c_str());
			ed::EndPin();
			ed::BeginPin(m_pbrNode->m_InputPin[4]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetMetallicNode()->GetNodeName()).c_str());
			ed::EndPin();
			ed::BeginPin(m_pbrNode->m_InputPin[5]->m_PinId, ed::PinKind::Input);
				ImGui::Text(("->" + mainFunction->GetEmissiveNode()->GetNodeName()).c_str());
			ed::EndPin();
			//ImGui::Text()
		ed::EndNode();
	}

	bool NodeGraph::CreateNewNodeMenu()
	{
		ed::Suspend();
		if (ed::ShowBackgroundContextMenu())
			ImGui::OpenPopup("CreateNewNode", ImGuiPopupFlags_MouseButtonRight);
		if (ImGui::BeginPopup("CreateNewNode"))
		{
			if (ImGui::MenuItem("ConstColor4"))
			{
				++m_uniqueId;
				//Create a ConstFloatValue Node, and add to m_pMaterial shaderfunction
				Ref<ConstFloatValue> floatValue4 = CreateRef<ConstFloatValue>(std::string("ConstColor4"), m_pMaterial, 4, false);
				//two phase initializer
				floatValue4->ConstrcutPutNodeAndSetPutNodeOwner();
				floatValue4->AddToMaterialOwner();

				//Create a Graph Node
				Ref<GraphNode> floatNode = CreateRef<GraphNode>();
				floatNode->m_NodeId = m_uniqueId;
				floatNode->p_Owner = floatValue4;
				
				//in terms of floatValue4 to create pin
				if (floatValue4->IsHaveInput())
				{
					floatNode->m_InputPin.reserve(floatValue4->GetInputNodeNum());
					for (uint32_t i = 0; i < floatValue4->GetOutputNodeNum(); ++i)
					{
						floatNode->m_InputPin[i] = CreateRef<GraphPin>();
						floatNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
						floatNode->m_InputPin[i]->m_LocationIndex = i;
						floatNode->m_InputPin[i]->m_Node = floatNode;
					}
				}
				if (floatValue4->IsHaveOutput())
				{
					floatNode->m_OutputPin.resize(floatValue4->GetOutputNodeNum());
					for (uint32_t i = 0; i < floatValue4->GetOutputNodeNum(); ++i)
					{
						floatNode->m_OutputPin[i] = CreateRef<GraphPin>();
						floatNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
						floatNode->m_OutputPin[i]->m_LocationIndex = i;
						floatNode->m_OutputPin[i]->m_Node = floatNode;
					}
				}
				m_Nodes.push_back(floatNode);
				ed::SetNodePosition(floatNode->m_NodeId, ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y)));
			}

			if (ImGui::MenuItem("Mul"))
			{
				++m_uniqueId;
				Ref<Mul> pMul = CreateRef<Mul>(std::string("Mul"), m_pMaterial);
				pMul->ConstrcutPutNodeAndSetPutNodeOwner();
				pMul->AddToMaterialOwner();

				Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
				pGraphNode->m_NodeId = m_uniqueId;
				pGraphNode->p_Owner = pMul;

				if (pMul->IsHaveInput())
				{
					pGraphNode->m_InputPin.resize(pMul->GetInputNodeNum());
					for (uint32_t i = 0; i < pGraphNode->m_InputPin.size(); ++i)
					{
						pGraphNode->m_InputPin[i] = CreateRef<GraphPin>();
						pGraphNode->m_InputPin[i]->m_PinId = ++m_uniqueId;
						pGraphNode->m_InputPin[i]->m_LocationIndex = i;
						pGraphNode->m_InputPin[i]->m_Node = pGraphNode;
					}
				}

				if (pMul->IsHaveOutput())
				{
					pGraphNode->m_OutputPin.resize(pMul->GetOutputNodeNum());
					for (uint32_t i = 0; i < pGraphNode->m_OutputPin.size(); ++i)
					{
						pGraphNode->m_OutputPin[i] = CreateRef<GraphPin>();
						pGraphNode->m_OutputPin[i]->m_PinId = ++m_uniqueId;
						pGraphNode->m_OutputPin[i]->m_LocationIndex = i;
						pGraphNode->m_OutputPin[i]->m_Node = pGraphNode;
					}
				}
				m_Nodes.push_back(pGraphNode);
				ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImVec2(m_MousePos.x, m_MousePos.y)));
			}
			ImGui::EndPopup();
		}

		ed::Resume();

		return true;
	}

	void NodeGraph::CreateNodePin(Ref<GraphNode> graphNode, uint32_t locationId)
	{
		graphNode->m_InputPin.push_back(CreateRef<GraphPin>());
		graphNode->m_InputPin.back()->m_PinId = ++m_uniqueId;
		graphNode->m_InputPin.back()->m_LocationIndex = locationId;
		graphNode->m_InputPin.back()->m_Node = graphNode;
	}
}