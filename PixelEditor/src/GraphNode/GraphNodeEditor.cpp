#include "pxpch.h"

#include "GraphNodeEditor.h"

#include "GraphPin.h"
#include "GraphLink.h"
#include "GraphNode.h"
#include "Builder.h"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/3D/Material/Material.h"
#include "Pixel/Renderer/3D/Material/ShaderFunction.h"
#include "Pixel/Renderer/3D/Material/ShaderMainFunction.h"
#include "Pixel/Renderer/3D/Material/InputNode.h"
#include "Pixel/Renderer/3D/Material/OutputNode.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"
#include "Pixel/Renderer/3D/Material/Mul.h"
#include "Pixel/Renderer/3D/Material/ConstFloatValue.h"
#include "Pixel/Renderer/3D/Material/Texture2DShaderFunction.h"
#include "Pixel/Renderer/3D/Material/TextureCoordinate.h"
#include "Pixel/Renderer/3D/Material/SinShaderFunction.h"
#include "Pixel/Renderer/3D/Material/ComponentMask.h"
#include "Pixel/Renderer/3D/Material/DotShaderFunction.h"
#include "Pixel/Renderer/3D/Material/CeilShaderFunction.h"
#include "Pixel/Renderer/3D/Material/LerpShaderFunction.h"
#include "Pixel/Renderer/3D/Material/ShaderStringFactory.h"
#include "Pixel/Scene/SceneSerializer.h"
#include "Pixel/Scene/SimpleScene.h"
#include "Pixel/Scene/Components/StaticMeshComponent.h"
#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/Material/MaterialInstance.h"
#include "Pixel/Core/KeyCodes.h"

//------other library------
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorCpuHandle.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "rapidjson/prettywriter.h"
//------other library------

#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/Buffer/GpuResource.h"

namespace Pixel {

	//extern const std::filesystem::path g_AssetPath;

	static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		//PIXEL_CORE_INFO("{0}, {1}", window->DC.CursorPos.x, window->DC.CursorPos.y);
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	GraphNodeEditor::GraphNodeEditor(const std::string& virtualPath, const std::string& physicalPath, Ref<Material> pMaterial, Ref<Framebuffer> pFinalColorFrameBuffer)
	{
		m_TopPanelHeight = 800.0f;
		m_DownPanelHeight = 400.0f;

		m_LeftPaneWidth = 200.0f;
		m_RightPanelWidth = 600.0f;

		m_HeaderBackgroundTexture = Texture2D::Create("Resources/Icons/BlueprintBackground.png");

		m_HeaderBackgroundTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		m_PreviewSceneTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		//------apply for preview intermediate nodes texture handle------
		m_PreviewIntermediateNodesTextureHandles.resize(128);

		for (size_t i = 0; i < 128; ++i)
		{
			m_PreviewIntermediateNodesTextureHandles[i] = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		}
		//------apply for preview intermediate nodes texture handle------

		Device::Get()->CopyDescriptorsSimple(1, m_HeaderBackgroundTextureHandle->GetCpuHandle(), m_HeaderBackgroundTexture->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

		//copy descriptor
		Device::Get()->CopyDescriptorsSimple(1, m_PreviewSceneTextureHandle->GetCpuHandle(), pFinalColorFrameBuffer->GetColorAttachmentDescriptorCpuHandle(0), DescriptorHeapType::CBV_UAV_SRV);

		m_BlueprintNodeBuilder = CreateRef<BlueprintNodeBuilder>((ImTextureID)(m_HeaderBackgroundTextureHandle->GetGpuHandle()->GetGpuPtr()), m_HeaderBackgroundTexture->GetWidth(), m_HeaderBackgroundTexture->GetHeight());

		ShaderStringFactory::m_ShaderValueIndex = 0;

		//if it is first open, then load the settings file
		ed::Config config;

		m_GraphNodeEditorConfigPath = "assets/materials/GraphNodeEditor/" + virtualPath + ".json";

		m_MaterialPhysicalPath = physicalPath;

		config.SettingsFile = m_GraphNodeEditorConfigPath.c_str();

		m_Editor = ed::CreateEditor(&config);

		//from the material to construct node��pin��link
		m_pMaterial = pMaterial;

		m_CreateNewNode = false;

		m_IsPreviewEventBlocked = false;

		//create the graph node
		for (size_t i = 0; i < m_pMaterial->GetShaderFunction().size(); ++i)
		{
			Ref<GraphNode> pGraphNode;
			pGraphNode = CreateRef<GraphNode>();
			pGraphNode->m_NodeId = m_pMaterial->GetShaderFunction()[i]->GetFunctioNodeId();
			pGraphNode->p_Owner = m_pMaterial->GetShaderFunction()[i];

			ShaderStringFactory::m_ShaderValueIndex = std::max(ShaderStringFactory::m_ShaderValueIndex, (uint32_t)pGraphNode->m_NodeId.Get());

			for (size_t j = 0; j < m_pMaterial->GetShaderFunction()[i]->GetInputNodeNum(); ++j)
			{
				Ref<GraphPin> pGraphPins = CreateRef<GraphPin>();
				pGraphPins->m_PinId = m_pMaterial->GetShaderFunction()[i]->GetInputNode(j)->GetPutNodeId();
				pGraphPins->m_PinName = m_pMaterial->GetShaderFunction()[i]->m_InputNodeDisplayName[j];
				pGraphPins->m_OwnerNode = pGraphNode;
				pGraphNode->m_InputPin.push_back(pGraphPins);
				m_GraphPins.push_back(pGraphPins);

				ShaderStringFactory::m_ShaderValueIndex = std::max(ShaderStringFactory::m_ShaderValueIndex, (uint32_t)pGraphPins->m_PinId.Get());
			}

			for (size_t j = 0; j < m_pMaterial->GetShaderFunction()[i]->GetOutputNodeNum(); ++j)
			{
				Ref<GraphPin> pGraphPins = CreateRef<GraphPin>();
				pGraphPins->m_PinId = m_pMaterial->GetShaderFunction()[i]->GetOutputNode(j)->GetPutNodeId();
				pGraphPins->m_PinName = m_pMaterial->GetShaderFunction()[i]->m_OutputNodeDisplayName[j];
				pGraphPins->m_OwnerNode = pGraphNode;
				pGraphNode->m_OutputPin.push_back(pGraphPins);
				m_GraphPins.push_back(pGraphPins);

				ShaderStringFactory::m_ShaderValueIndex = std::max(ShaderStringFactory::m_ShaderValueIndex, (uint32_t)pGraphPins->m_PinId.Get());
			}

			m_GraphNodes.push_back(pGraphNode);
		}

		for (auto& item : m_pMaterial->GetLinks())
		{
			uint32_t inputPinId = item.x;
			uint32_t outputPinId = item.y;

			Ref<GraphLink> pGraphLinks = CreateRef<GraphLink>();
			for (size_t i = 0; i < m_GraphPins.size(); ++i)
			{		
				if (m_GraphPins[i]->m_PinId.Get() == inputPinId)
				{
					pGraphLinks->m_InputPin = m_GraphPins[i];
					m_GraphPins[i]->m_NodeLink = pGraphLinks;
				}
				if (m_GraphPins[i]->m_PinId.Get() == outputPinId)
				{
					pGraphLinks->m_OutputPin = m_GraphPins[i];
					m_GraphPins[i]->m_NodeLink = pGraphLinks;
				}
			}
			++ShaderStringFactory::m_ShaderValueIndex;
			pGraphLinks->m_LinkId = ShaderStringFactory::m_ShaderValueIndex;
			m_GraphLinks.push_back(pGraphLinks);
		}

		//------create simple scene------
		m_pPreviewScene = CreateRef<SimpleScene>();
		//------create simple scene------
	}

	GraphNodeEditor::~GraphNodeEditor()
	{
	}

	void GraphNodeEditor::OnImGuiRender(bool& OpenGraphNodeEditor)
	{
		//when open, will from the material asset open, then load the graph editor json file
		
		ImGui::Begin("Graph Node Editor", &OpenGraphNodeEditor);

		ImVec2 content = ImGui::GetContentRegionAvail();
		if(m_TopPanelHeight > content.y)
		{
			m_TopPanelHeight = content.y / 4.0f;
			m_DownPanelHeight = content.y - m_TopPanelHeight;
		}
		//PIXEL_CORE_INFO("{0}, {1}", content.x, content.y);
		//render
		ed::SetCurrentEditor(m_Editor);
			Splitter(false, 4.0f, &m_TopPanelHeight, &m_DownPanelHeight, 10.0f, 10.0f);
			DrawTopPanel(m_TopPanelHeight + 4.0f);
			Splitter(true, 4.0f, &m_LeftPaneWidth, &m_RightPanelWidth, 10.0f, 10.0f);
			DrawLeftPreViewScenePanel(m_LeftPaneWidth);
			ImGui::SameLine(0.0f, 10.0f);
			//ImGui::BeginHorizontal("BeginHorizontal##");
			//ImGui::BeginChild("PreviewScene", ImVec2(m_LeftPaneWidth, 0.0f));
			////draw preview scene
			//ImGui::Image((ImTextureID)m_PreviewSceneTextureHandle->GetGpuHandle()->GetGpuPtr(), ImVec2(256, 256));
			//ImGui::EndChild();
			//ImGui::EndHorizontal();
			//ImGui::BeginHorizontal("graph editor##");
			ed::Begin("Graph Node Editor Canvas");

			if (ImGui::IsKeyPressed(KeyCodes::PX_KEY_F))
			{
				ed::NavigateToContent();
			}
			
			//ImVec2 content2 = ImGui::GetContentRegionAvail();
			//PIXEL_CORE_INFO("{0}, {1}", content2.x, content2.y);

			//draw shader main function

			//TODO:in the future, will in terms of the shading model to switch this
			DrawMainFunctionNode();

			//1.draw node
			DrawNodes();

			//2.draw link
			DrawLinks();

			//3.handle create links and delete nodes and delete links
			HandleInteraction();		
			ed::End();
			//ImGui::EndHorizontal();
		ed::SetCurrentEditor(nullptr);


		ImGui::End();
	}

	void GraphNodeEditor::OnUpdate(Timestep& ts, EditorCamera& editorCamera, Ref<Framebuffer> pGeoFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, Ref<Framebuffer> pFinalFrameBuffer)
	{
		//draw preview mesh
		m_pPreviewScene->OnUpdateEditorDeferred(ts, editorCamera, pGeoFrameBuffer, pLightFrameBuffer, pFinalFrameBuffer, m_pMaterial);
	}

	bool GraphNodeEditor::IsPreviewSceneEventBlocked()
	{
		return m_IsPreviewEventBlocked;
	}

	void GraphNodeEditor::DrawMainFunctionNode()
	{
		//ed::BeginNode(1);
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
				m_BlueprintNodeBuilder->Begin(m_GraphNodes[i]->m_NodeId);
				glm::vec3 headerColor = m_GraphNodes[i]->p_Owner->m_HeaderColor;
				//draw header
				m_BlueprintNodeBuilder->Header(ImVec4(headerColor.r, headerColor.g, headerColor.b, 1.0f));
				//m_BlueprintNodeBuilder->Header();
				ImGui::Spring(0);
				ImGui::TextUnformatted(m_GraphNodes[i]->p_Owner->GetShowName().c_str());
				ImGui::Spring(1);
				ImGui::Dummy(ImVec2(0, 28));
				m_BlueprintNodeBuilder->EndHeader();
				for (size_t j = 0; j < m_GraphNodes[i]->m_InputPin.size(); ++j)
				{
					m_BlueprintNodeBuilder->Input(m_GraphNodes[i]->m_InputPin[j]->m_PinId);
					DrawPinIcon(m_GraphNodes[i]->m_InputPin[j]->m_Color, ImColor(32, 32, 32, 255));
					ImGui::Spring(0);
					ImGui::TextUnformatted(m_GraphNodes[i]->m_InputPin[j]->m_PinName.c_str());
					ImGui::Spring(0);
					m_BlueprintNodeBuilder->EndInput();
				}
				m_BlueprintNodeBuilder->End();
			}
		}
		//ed::EndNode();
	}

	void GraphNodeEditor::DrawTopPanel(float panelHeight)
	{
		ImGui::BeginChild("TopPanel", ImVec2(0, panelHeight));
		ImGui::BeginHorizontal("TopPanel");
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, panelHeight / 8.0f);
		//if(ImGui::Button("Compiler", ImVec2(0, std::max(panelHeight - 8.0f, 0.0f))))
		//{
		//	//generate and compiler
		//
		//	m_pMaterial->GetMainFunction()->ClearShaderTreeStringFlag();
		//	//call the material's get shader tree string, then save the shader to assets/shaders/ShaderGraph
		//	std::string out = ShaderStringFactory::CreateDeferredGeometryShaderString(m_pMaterial, m_pPreviewScene->GetPreviewModel().m_Model->GetMeshes()[0]);
		//	PIXEL_CORE_INFO(out);
		//}
		if(ImGui::Button("Save", ImVec2(0, std::max(panelHeight - 8.0f, 0.0f))))
		{
			//save will also call the compiler
			//m_pMaterial->GetMainFunction()->ClearShaderTreeStringFlag();
			//call the material's get shader tree string, then save the shader to assets/shaders/ShaderGraph
			//std::string out = ShaderStringFactory::CreateDeferredGeometryShaderString(m_pMaterial, m_pPreviewScene->GetPreviewModel().m_Model->GetMeshes()[0]);
			//PIXEL_CORE_INFO(out);

			//save the material
			rapidjson::StringBuffer strBuf;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
			writer.StartObject();
			rttr::type materialType = rttr::type::get<Material>();
			writer.Key(materialType.get_name().to_string().c_str());
			SceneSerializer::ToJsonRecursive(*m_pMaterial, writer, true);
			writer.EndObject();
			std::string data = strBuf.GetString();
			std::ofstream fout(m_MaterialPhysicalPath);
			fout << data.c_str();
			fout.close();

			//------update preview scene------
			m_pPreviewScene->SetModelMaterial(m_pMaterial);
			//------update preview scene------

			//notify reference to this material's material instance to construct parameter
			auto& materialInstances = AssetManager::GetSingleton().GetMaterialInstances();

			for (auto& materialInstance : materialInstances)
			{
				if (materialInstance.second->GetMaterial() == m_pMaterial)
				{
					//reconstruct
					materialInstance.second->ReConstructParameter(m_pMaterial);

					//resave the material instance to disk

					break;
				}
			}
		}

		if (ImGui::Button("PreviewNodes", ImVec2(0, std::max(panelHeight - 8.0f, 0.0f))))
		{
			ShaderStringFactory::m_GenerateIntermediateNodesResult = true;

			//save the intermediate nodes shader in the shader function(TODO:save the shader in the graph node good?)
			ShaderStringFactory::GenerateIntermediateShaderString(m_pMaterial, m_pPreviewScene->GetPreviewModel().m_Model->GetMeshes()[0]);

			for (size_t i = 0; i < m_GraphNodes.size(); ++i)
			{
				//get owner shader function
				Ref<ShaderFunction> pShaderFunction = m_GraphNodes[i]->p_Owner;

				if (pShaderFunction->GetOutputNodeNum() == 0) continue;//skip shader main function

				Ref<Context> pFinalColorContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);

				FramebufferSpecification fbSpec;
				fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
				fbSpec.Width = 128.0f;
				fbSpec.Height = 128.0f;

				m_GraphNodes[i]->m_PreviewTexture = Framebuffer::Create(fbSpec);

				std::string& ShaderCode = pShaderFunction->m_IntermediateShaderString;

				//PIXEL_CORE_INFO("{0}******", ShaderCode);

				std::string shaderPath = "assets/shaders/cache/previewNode/" + m_pMaterial->GetMaterialName() + std::to_string(i) + ".hlsl";
				//save to disk
				std::ofstream fstream(shaderPath);
				fstream << ShaderCode;
				fstream.close();

				Ref<Shader> pVertexShader = Shader::Create(shaderPath, "VS", "vs_5_0", false, m_pMaterial);
				Ref<Shader> pPixelShader = Shader::Create(shaderPath, "PS", "ps_5_0", false, m_pMaterial);

				//draw to m_PreviewTexture
				
				Application::Get().GetRenderer()->DrawIntermediatePreviewNodes(pFinalColorContext, pVertexShader, pPixelShader, m_GraphNodes[i]->m_PreviewTexture, m_pMaterial);

				pFinalColorContext->Finish(true);
			}

			ShaderStringFactory::m_GenerateIntermediateNodesResult = false;
		}

		ImGui::PopStyleVar(1);
		ImGui::EndHorizontal();
		ImGui::EndChild();
	}

	void GraphNodeEditor::DrawLeftPreViewScenePanel(float panelWidth)
	{
		ImGui::BeginChild("AttributeAndPreviewScenePanel", ImVec2(panelWidth, 0.0f));		
		ImGui::BeginChild("PreviewScenePanel", ImVec2(256, 256));
		ImGui::Image((ImTextureID)m_PreviewSceneTextureHandle->GetGpuHandle()->GetGpuPtr(), ImVec2(256, 256));
		auto itemHovered = ImGui::IsItemHovered();
		auto itemFocused = ImGui::IsItemFocused();
		m_IsPreviewEventBlocked = itemHovered || itemFocused;
		ImGui::EndChild();

		std::vector<ed::NodeId> selectedNodes;
		selectedNodes.resize(ed::GetSelectedObjectCount());
		int32_t nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int32_t>(selectedNodes.size()));
		selectedNodes.resize(nodeCount);

		//iterator over the all nodes, then get the selected node
		for (auto& node : m_GraphNodes)
		{
			bool IsSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node->m_NodeId) != selectedNodes.end();
			if (IsSelected)
			{
				m_CurrentSelectedGraphNode = node;
			}
		}

		if (m_CurrentSelectedGraphNode != nullptr)
		{
			//show details
			Ref<ShaderFunction> pShaderFunction = m_CurrentSelectedGraphNode->p_Owner;

			//if (pShaderFunction->GetShaderFunctionType() == ShaderFunctionType::ConstFloatValue4)
			//{
			//	Ref<ConstFloatValue> pConstFloatValue4ShaderFuntion = std::static_pointer_cast<ConstFloatValue>(pShaderFunction);
			//	
			//	ImGui::ColorEdit4("const float value 4", pConstFloatValue4ShaderFuntion->m_Value.data());
			//}
			static char buf[256];

			if (rttr::type::get(*pShaderFunction) == rttr::type::get<ConstFloatValue>())
			{
				Ref<ConstFloatValue> pConstFloatValueShaderFuntion = std::static_pointer_cast<ConstFloatValue>(pShaderFunction);

				if (pConstFloatValueShaderFuntion->m_Value.size() == 4)
				{
					if (pConstFloatValueShaderFuntion->m_bIsCustom)
					{
						memset(buf, 0, sizeof(buf));
						strcpy_s(buf, sizeof(buf), pConstFloatValueShaderFuntion->GetShowName().c_str());
						ImGui::Separator();
						ImGui::Text("parameter name");
						ImGui::SameLine();
						//edit parameter name and texture default parameter
						if (ImGui::InputText("##parameter name", buf, 256))
						{
							std::string str = std::string(buf);
							pConstFloatValueShaderFuntion->SetShowName(str);//edit parameter name
							pConstFloatValueShaderFuntion->ResetInShaderName();
						}
						//ImGui::Separator();
					}

					ImGui::Separator();
					ImGui::Text("value");
					ImGui::SameLine();
					ImGui::InputFloat4("##const float value 4", pConstFloatValueShaderFuntion->m_Value.data(), "%.3f");
					//ImGui::ColorEdit4("##const float value 4", pConstFloatValueShaderFuntion->m_Value.data(), ImGuiColorEditFlags_Float);//TODO:modify to [0.0f, 1.0f]?
				}
			}
			else if (rttr::type::get(*pShaderFunction) == rttr::type::get<Texture2DShaderFunction>())
			{
				Ref<Texture2DShaderFunction> pTexture2DShaderFunction = std::static_pointer_cast<Texture2DShaderFunction>(pShaderFunction);
				
				memset(buf, 0, sizeof(buf));
				strcpy_s(buf, sizeof(buf), pTexture2DShaderFunction->GetShowName().c_str());
				ImGui::Separator();
				ImGui::Text("parameter name");
				ImGui::SameLine();
				//edit parameter name and texture default parameter
				if (ImGui::InputText("##parameter name", buf, 256))
				{
					std::string str = std::string(buf);
					pTexture2DShaderFunction->SetShowName(str);//edit parameter name
				}

				ImGui::Text("Texture Parameter");
				ImGui::SameLine();
				//edit texture
				if (ImGui::BeginCombo("##Texture Parameter", pTexture2DShaderFunction->m_TextureVirtualPath.c_str()))
				{
					//for(auto& item :)
					auto& textures = AssetManager::GetSingleton().GetTextureAssetRegistry();

					for (auto& item : textures)
					{
						bool isSelected = item.first == pTexture2DShaderFunction->m_TextureVirtualPath;
						if (ImGui::Selectable(item.first.c_str(), isSelected))
						{
							pTexture2DShaderFunction->m_TextureVirtualPath = item.first;//set to it
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Text("IsNormal");
				ImGui::SameLine();
				ImGui::Checkbox("##IsNormal", &(pTexture2DShaderFunction->GetDecodedNormal()));
			}
			else if (rttr::type::get(*pShaderFunction) == rttr::type::get<ComponentMask>())
			{
				Ref<ComponentMask> pComponentMaskShaderFunction = std::static_pointer_cast<ComponentMask>(pShaderFunction);
				ImGui::Separator();
				//bool rEnable, gEnable, bEnable, aEnable;
				ImGui::Checkbox("R", &pComponentMaskShaderFunction->R);

				ImGui::Checkbox("G", &pComponentMaskShaderFunction->G);

				ImGui::Checkbox("B", &pComponentMaskShaderFunction->B);

				ImGui::Checkbox("A", &pComponentMaskShaderFunction->A);

				pComponentMaskShaderFunction->UpdateOutputNodeValueType();
			}
		}

		ImGui::EndChild();
	}

	void GraphNodeEditor::DrawLinks()
	{
		//draw links
		for (auto& linkInfo : m_GraphLinks)
			ed::Link(linkInfo->m_LinkId, linkInfo->m_InputPin.lock()->m_PinId, linkInfo->m_OutputPin.lock()->m_PinId);
	}

	void GraphNodeEditor::DrawNodes()
	{
		uint32_t currentNodeIndex = 0;
		for(auto& node : m_GraphNodes)
		{
			//skip the main node
			if (node->m_NodeId.Get() == 1)
				continue;
			m_BlueprintNodeBuilder->Begin(node->m_NodeId);

			glm::vec3 headerColor = node->p_Owner->m_HeaderColor;
			//draw header
			m_BlueprintNodeBuilder->Header(ImVec4(headerColor.r, headerColor.g, headerColor.b, 1.0f));
			ImGui::Spring(0);
			ImGui::TextUnformatted(node->p_Owner->GetShowName().c_str());
			ImGui::Spring(1);
			ImGui::Dummy(ImVec2(0, 28));
			ImGui::Spring(0);
			m_BlueprintNodeBuilder->EndHeader();

			uint32_t i = 0;
			//draw input pin
			for(auto& input : node->m_InputPin)
			{
				m_BlueprintNodeBuilder->Input(input->m_PinId);
				glm::vec3 color = node->p_Owner->m_InputNodeDisplayColor[i];
				DrawPinIcon(ImGui::ColorConvertFloat4ToU32(ImVec4(color.r, color.g, color.b, 1.0f)), ImColor(32, 32, 32, 255));
				ImGui::Spring(0);
				ImGui::TextUnformatted(input->m_PinName.c_str());
				m_BlueprintNodeBuilder->EndInput();
				++i;
			}

			//------draw intermediate nodes result------
			if (node->m_PreviewTexture != nullptr)
			{
				Device::Get()->CopyDescriptorsSimple(1, m_PreviewIntermediateNodesTextureHandles[currentNodeIndex]->GetCpuHandle(), node->m_PreviewTexture->GetColorAttachmentDescriptorCpuHandle(0), DescriptorHeapType::CBV_UAV_SRV);
				ImGui::Image((ImTextureID)m_PreviewIntermediateNodesTextureHandles[currentNodeIndex]->GetGpuHandle()->GetGpuPtr(), ImVec2(128.0f, 128.0f));
			}
			//------draw intermediate nodes result------

			i = 0;
			//draw output pin
			for(auto& output : node->m_OutputPin)
			{
				m_BlueprintNodeBuilder->Output(output->m_PinId);
				ImGui::TextUnformatted(output->m_PinName.c_str());		
				ImGui::Spring(0);
				glm::vec3 color = node->p_Owner->m_OutputNodeDisplayColor[i];
				DrawPinIcon(ImGui::ColorConvertFloat4ToU32(ImVec4(color.r, color.g, color.b, 1.0f)), ImColor(32, 32, 32, 255));
				m_BlueprintNodeBuilder->EndOutput();
				++i;
			}
			m_BlueprintNodeBuilder->End();

			++currentNodeIndex;
		}
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

	void GraphNodeEditor::HandleInteraction()
	{

		if(ed::BeginCreate(ImColor(204, 232, 207, 255), 2.0f))
		{
			ed::PinId outputPinId = 0, inputPinId = 0;

			//------create link------
			if(ed::QueryNewLink(&outputPinId, &inputPinId))
			{
				auto outputPin = FindPin(outputPinId);
				auto inputPin = FindPin(inputPinId);

				//one output pin could link multiple input pin
				/*
				bool alreadyLink = false;

				//check the outputpin and inputpin don't have link
				for(size_t i = 0; i < m_GraphLinks.size(); ++i)
				{
					if (m_GraphLinks[i]->m_InputPin.lock() == inputPin)
						alreadyLink = true;
					if (m_GraphLinks[i]->m_OutputPin.lock() == outputPin)
						alreadyLink = true;
				}
				*/

				ShowLabel("+ Create Link", ImColor(32, 45, 32, 100));
				if (ed::AcceptNewItem(ImColor(128, 255, 128, 255), 4.0f))
				{
					//create new link
					Ref<GraphLink> pGraphLink = CreateRef<GraphLink>();
					pGraphLink->m_InputPin = inputPin;
					pGraphLink->m_OutputPin = outputPin;
					inputPin->m_NodeLink = pGraphLink;
					outputPin->m_NodeLink = pGraphLink;
					pGraphLink->m_LinkId = ++ShaderStringFactory::m_ShaderValueIndex;
					//add new graph link to m_GraphLinks
					m_GraphLinks.push_back(pGraphLink);
					//create logic link
					uint32_t inputPinLocationIndex = 0, outputPinLocationIndex = 0;
					if (inputPin->m_OwnerNode.lock())
					{
						for (size_t i = 0; i < inputPin->m_OwnerNode.lock()->m_InputPin.size(); ++i)
						{
							if (inputPin->m_OwnerNode.lock()->m_InputPin[i] == inputPin)
							{
								inputPinLocationIndex = i;
								break;
							}
						}
					}
					if (outputPin->m_OwnerNode.lock())
					{
						for (size_t i = 0; i < outputPin->m_OwnerNode.lock()->m_OutputPin.size(); ++i)
						{
							if (outputPin->m_OwnerNode.lock()->m_OutputPin[i] == outputPin)
							{
								outputPinLocationIndex = i;
								break;
							}
						}
					}
					//------link two nodes------
					Ref<InputNode> inputNode = inputPin->m_OwnerNode.lock()->p_Owner->GetInputNode(inputPinLocationIndex);
					Ref<OutputNode> outputNode = outputPin->m_OwnerNode.lock()->p_Owner->GetOutputNode(outputPinLocationIndex);
					inputNode->Connection(outputNode);
					//------link two nodes------

					m_pMaterial->GetLinks().push_back(glm::vec2(inputPin->m_PinId.Get(), outputPin->m_PinId.Get()));
				}
			}
			//------create link------
		}
		ed::EndCreate();

		if (ed::BeginDelete())
		{
			ed::LinkId linkId = 0;
			
			while (ed::QueryDeletedLink(&linkId))
			{
				if (ed::AcceptDeletedItem())
				{
					//Ref<GraphLink> pGraphLink = FindLink(linkId);
					//
					//Ref<GraphPin> pGraphInputPin = pGraphLink->m_InputPin.lock();
					//Ref<GraphPin> pGraphOutputPin = pGraphLink->m_OutputPin.lock();

				}
			}

			ed::NodeId nodeId = 0;

			while (ed::QueryDeletedNode(&nodeId))
			{
				if (ed::AcceptDeletedItem())
				{
					//find owner
					Ref<GraphNode> pGrapNodes = FindNode(nodeId);

					//find all graph links, then delete
					std::vector<Ref<GraphPin>> InputPins = pGrapNodes->m_InputPin;
					std::vector<Ref<GraphPin>> OutputPins = pGrapNodes->m_OutputPin;

					std::vector<Ref<GraphLink>> needDeletedLinks;
					for (size_t i = 0; i < InputPins.size(); ++i)
					{
						if (InputPins[i]->m_NodeLink != nullptr)
						{
							Ref<GraphLink> pNodeLinks = InputPins[i]->m_NodeLink;//node link
							Ref<GraphPin> pOutputPin = pNodeLinks->m_OutputPin.lock();
							if (pOutputPin != nullptr)
							{
								pOutputPin->m_NodeLink = nullptr;
							}
							needDeletedLinks.push_back(pNodeLinks);
						}				
					}

					for (size_t i = 0; i < OutputPins.size(); ++i)
					{
						if (OutputPins[i]->m_NodeLink != nullptr)
						{
							Ref<GraphLink> pNodeLinks = OutputPins[i]->m_NodeLink;//node link
							Ref<GraphPin> pInputPin = pNodeLinks->m_InputPin.lock();
							if (pInputPin != nullptr)
							{
								pInputPin->m_NodeLink = nullptr;
							}
							needDeletedLinks.push_back(pNodeLinks);
						}				
					}

					std::vector<glm::ivec2>& logicLinks = m_pMaterial->GetLinks();

					logicLinks.erase(std::remove_if(logicLinks.begin(), logicLinks.end(), [&InputPins, &OutputPins](glm::ivec2& link) {
						for (size_t i = 0; i < InputPins.size(); ++i)
						{
							if (InputPins[i]->m_PinId.Get() == link.x || InputPins[i]->m_PinId.Get() == link.y)
								return true;
						}
						for (size_t i = 0; i < OutputPins.size(); ++i)
						{
							if (OutputPins[i]->m_PinId.Get() == link.x || OutputPins[i]->m_PinId.Get() == link.y)
								return true;
						}
						return false;
					}), logicLinks.end());

					m_GraphPins.erase(std::remove_if(m_GraphPins.begin(), m_GraphPins.end(), [&InputPins](Ref<GraphPin> pGraphPin) { 
						for (size_t i = 0; i < InputPins.size(); ++i)
						{
							if (InputPins[i] == pGraphPin) return true;
						}
						return false;
					}), m_GraphPins.end());

					m_GraphPins.erase(std::remove_if(m_GraphPins.begin(), m_GraphPins.end(), [&OutputPins](Ref<GraphPin> pGraphPin) {
						for (size_t i = 0; i < OutputPins.size(); ++i)
						{
							if (OutputPins[i] == pGraphPin) return true;
						}
						return false;
					}), m_GraphPins.end());

					InputPins.clear();
					OutputPins.clear();

					m_GraphLinks.erase(std::remove_if(m_GraphLinks.begin(), m_GraphLinks.end(), [&needDeletedLinks](Ref<GraphLink> pGraphLink) {
						for (size_t i = 0; i < needDeletedLinks.size(); ++i)
						{
							if (needDeletedLinks[i] == pGraphLink) return true;
						}
						return false;
					}), m_GraphLinks.end());

					auto iter = std::find_if(m_GraphNodes.begin(), m_GraphNodes.end(), [&pGrapNodes](Ref<GraphNode> pGraphNode) {
						return pGraphNode == pGrapNodes;
					});
					//delete graph node
					m_GraphNodes.erase(iter);

					for (size_t i = 0; i < m_pMaterial->m_pShaderFunctionArray.size(); ++i)
					{
						if (m_pMaterial->m_pShaderFunctionArray[i] == pGrapNodes->p_Owner)
						{
							m_pMaterial->DeleteShaderFunction(m_pMaterial->m_pShaderFunctionArray[i]);
							break;
						}
					}

					m_CurrentSelectedGraphNode = nullptr;//TODO:use weak_ptr
				}
			}

			ed::EndDelete();
		}

		//create new node
		CreateNewNodes();

		ShowNodeContextMenu();
	}

	void GraphNodeEditor::ShowLabel(const std::string& label, ImColor color)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());

		auto size = ImGui::CalcTextSize(label.c_str());

		auto padding = ImGui::GetStyle().FramePadding;
		auto spacing = ImGui::GetStyle().ItemSpacing;

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

		auto rectMin = ImGui::GetCursorScreenPos() - padding;
		auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

		auto drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
		ImGui::TextUnformatted(label.c_str());
	}

	void GraphNodeEditor::CreateNewNodes()
	{
		ed::Suspend();
		if (ed::ShowBackgroundContextMenu())
		{
			ImGui::OpenPopup("Create New Node");
		}

		if(ImGui::BeginPopup("Create New Node"))
		{
			if(ImGui::MenuItem("Mul"))
			{
				//create mul node
				CreateMul();
			}

			if (ImGui::MenuItem("ConstFloatValue4"))
			{
				CreateConstFloatValue4();
			}

			if (ImGui::MenuItem("Texture2D"))
			{
				CreateTexture2D(textureParameterSuffix);
				++textureParameterSuffix;
			}

			if (ImGui::MenuItem("TexCoordinate"))
			{
				CreateTexcoordinate();
			}

			if (ImGui::MenuItem("Sin"))
			{
				CreateSin();
			}

			if (ImGui::MenuItem("ComponentMask"))
			{
				CreateComponentMask();
			}

			if (ImGui::MenuItem("Dot"))
			{
				CreateDot();
			}

			if (ImGui::MenuItem("Ceil"))
			{
				CreateCeil();
			}

			if (ImGui::MenuItem("Lerp"))
			{
				CreateLerp();
			}

			ImGui::EndPopup();
		}
		ed::Resume();
	}

	void GraphNodeEditor::DrawIntermediatePreviewNodes(Ref<GraphNode> pGraphNode)
	{
		//draw preview graph node

	}

	void GraphNodeEditor::ShowNodeContextMenu()
	{
		ed::Suspend();
		if (m_CurrentSelectedGraphNode != nullptr)
		{
			if (ed::ShowNodeContextMenu(&(m_CurrentSelectedGraphNode->m_NodeId)))
				ImGui::OpenPopup("Node Context Menu");
		}		
		ed::Resume();

		ed::Suspend();
		if (ImGui::BeginPopup("Node Context Menu"))
		{		
			if (m_CurrentSelectedGraphNode != nullptr)
			{
				Ref<ShaderFunction> pShaderFunction = m_CurrentSelectedGraphNode->p_Owner;

				//if it's type is const float value, then can promote to parameter
				if (rttr::type::get(*pShaderFunction) == rttr::type::get<ConstFloatValue>())
				{
					if (ImGui::MenuItem("promote to parameter"))
					{
						std::static_pointer_cast<ConstFloatValue>(pShaderFunction)->m_bIsCustom = true;
					}
				}
			}			

			ImGui::EndPopup();
		}
		ed::Resume();
	}

	void GraphNodeEditor::CreateMul()
	{
		Ref<ShaderFunction> pMul = CreateRef<Mul>("Mul", m_pMaterial);
		pMul->AddToMaterialOwner();
		pMul->ConstructPutNodeAndSetPutNodeOwner();
		pMul->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pMul->GetFunctioNodeId();
		pGraphNode->p_Owner = pMul;

		for(size_t i = 0; i < pMul->GetInputNodeNum(); ++i)
		{
			pMul->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pMul->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pMul->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for(size_t i = 0; i < pMul->GetOutputNodeNum(); ++i)
		{
			pMul->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pMul->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pMul->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		//PIXEL_CORE_INFO("{0}, {1}", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pMul);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateConstFloatValue4()
	{
		Ref<ShaderFunction> pConstFloatValue4 = CreateRef<ConstFloatValue>("ConstFloatValue4", m_pMaterial, 4, false);
		pConstFloatValue4->AddToMaterialOwner();
		pConstFloatValue4->ConstructPutNodeAndSetPutNodeOwner();
		pConstFloatValue4->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pConstFloatValue4->GetFunctioNodeId();
		pGraphNode->p_Owner = pConstFloatValue4;

		for (size_t i = 0; i < pConstFloatValue4->GetInputNodeNum(); ++i)
		{
			pConstFloatValue4->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pConstFloatValue4->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pConstFloatValue4->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pConstFloatValue4->GetOutputNodeNum(); ++i)
		{
			pConstFloatValue4->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pConstFloatValue4->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pConstFloatValue4->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pConstFloatValue4);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateTexture2D(uint32_t textureParameterSuffix)
	{
		Ref<ShaderFunction> pTexture = CreateRef<Texture2DShaderFunction>("TextureParameter" + std::to_string(textureParameterSuffix), m_pMaterial);
		pTexture->AddToMaterialOwner();
		pTexture->ConstructPutNodeAndSetPutNodeOwner();
		pTexture->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pTexture->GetFunctioNodeId();
		pGraphNode->p_Owner = pTexture;

		for (size_t i = 0; i < pTexture->GetInputNodeNum(); ++i)
		{
			pTexture->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pTexture->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pTexture->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pTexture->GetOutputNodeNum(); ++i)
		{
			pTexture->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pTexture->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pTexture->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pTexture);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateTexcoordinate()
	{
		Ref<ShaderFunction> pTexCoordinate = CreateRef<TextureCoordinate>("TextureCoordinate", m_pMaterial);
		pTexCoordinate->AddToMaterialOwner();
		pTexCoordinate->ConstructPutNodeAndSetPutNodeOwner();
		pTexCoordinate->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pTexCoordinate->GetFunctioNodeId();
		pGraphNode->p_Owner = pTexCoordinate;

		for (size_t i = 0; i < pTexCoordinate->GetInputNodeNum(); ++i)
		{
			pTexCoordinate->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pTexCoordinate->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pTexCoordinate->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pTexCoordinate->GetOutputNodeNum(); ++i)
		{
			pTexCoordinate->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pTexCoordinate->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pTexCoordinate->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pTexCoordinate);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateSin()
	{
		Ref<ShaderFunction> pSin = CreateRef<SinShaderFunction>("Sin", m_pMaterial);
		pSin->AddToMaterialOwner();
		pSin->ConstructPutNodeAndSetPutNodeOwner();
		pSin->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pSin->GetFunctioNodeId();
		pGraphNode->p_Owner = pSin;

		for (size_t i = 0; i < pSin->GetInputNodeNum(); ++i)
		{
			pSin->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pSin->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pSin->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pSin->GetOutputNodeNum(); ++i)
		{
			pSin->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pSin->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pSin->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pSin);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateComponentMask()
	{
		Ref<ShaderFunction> pComponentMask = CreateRef<ComponentMask>("Mask", m_pMaterial);
		pComponentMask->AddToMaterialOwner();
		pComponentMask->ConstructPutNodeAndSetPutNodeOwner();
		pComponentMask->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pComponentMask->GetFunctioNodeId();
		pGraphNode->p_Owner = pComponentMask;

		for (size_t i = 0; i < pComponentMask->GetInputNodeNum(); ++i)
		{
			pComponentMask->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pComponentMask->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pComponentMask->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pComponentMask->GetOutputNodeNum(); ++i)
		{
			pComponentMask->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pComponentMask->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pComponentMask->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pComponentMask);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateDot()
	{
		Ref<ShaderFunction> pDot = CreateRef<DotShaderFunction>("Dot", m_pMaterial);
		pDot->AddToMaterialOwner();
		pDot->ConstructPutNodeAndSetPutNodeOwner();
		pDot->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pDot->GetFunctioNodeId();
		pGraphNode->p_Owner = pDot;

		for (size_t i = 0; i < pDot->GetInputNodeNum(); ++i)
		{
			pDot->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pDot->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pDot->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pDot->GetOutputNodeNum(); ++i)
		{
			pDot->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pDot->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pDot->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pDot);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateCeil()
	{
		Ref<ShaderFunction> pCeil = CreateRef<CeilShaderFunction>("Ceil", m_pMaterial);
		pCeil->AddToMaterialOwner();
		pCeil->ConstructPutNodeAndSetPutNodeOwner();
		pCeil->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pCeil->GetFunctioNodeId();
		pGraphNode->p_Owner = pCeil;

		for (size_t i = 0; i < pCeil->GetInputNodeNum(); ++i)
		{
			pCeil->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pCeil->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pCeil->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pCeil->GetOutputNodeNum(); ++i)
		{
			pCeil->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pCeil->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pCeil->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pCeil);
		//------add to material shader function------
	}

	void GraphNodeEditor::CreateLerp()
	{
		Ref<ShaderFunction> pLerp = CreateRef<LerpShaderFunction>("Lerp", m_pMaterial);
		pLerp->AddToMaterialOwner();
		pLerp->ConstructPutNodeAndSetPutNodeOwner();
		pLerp->SetFunctionNodeId(++ShaderStringFactory::m_ShaderValueIndex);

		//in terms the logic node to create graph node and push to graph node's vector
		Ref<GraphNode> pGraphNode = CreateRef<GraphNode>();
		pGraphNode->m_NodeId = pLerp->GetFunctioNodeId();
		pGraphNode->p_Owner = pLerp;

		for (size_t i = 0; i < pLerp->GetInputNodeNum(); ++i)
		{
			pLerp->GetInputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> inputPin = CreateRef<GraphPin>();
			inputPin->m_PinId = pLerp->GetInputNode(i)->GetPutNodeId();
			inputPin->m_OwnerNode = pGraphNode;
			inputPin->m_PinName = pLerp->m_InputNodeDisplayName[i];
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pLerp->GetOutputNodeNum(); ++i)
		{
			pLerp->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pLerp->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pLerp->m_OutputNodeDisplayName[i];
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		ed::SetNodePosition(pGraphNode->m_NodeId, ed::ScreenToCanvas(ImGui::GetMousePos()));

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pLerp);
		//------add to material shader function------
	}

	Ref<GraphPin> GraphNodeEditor::FindPin(ed::PinId pinId)
	{
		for (size_t i = 0; i < m_GraphPins.size(); ++i)
		{
			if (m_GraphPins[i]->m_PinId == pinId)
				return m_GraphPins[i];
		}
		return nullptr;
	}
	Ref<GraphLink> GraphNodeEditor::FindLink(ed::LinkId linkId)
	{
		for (size_t i = 0; i < m_GraphLinks.size(); ++i)
		{
			if (m_GraphLinks[i]->m_LinkId == linkId)
				return m_GraphLinks[i];
		}
		return nullptr;
	}
	Ref<GraphNode> GraphNodeEditor::FindNode(ed::NodeId nodeId)
	{
		for (size_t i = 0; i < m_GraphNodes.size(); ++i)
		{
			if (m_GraphNodes[i]->m_NodeId == nodeId)
				return m_GraphNodes[i];
		}
		return nullptr;
	}
}
