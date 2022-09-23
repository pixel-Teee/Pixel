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
#include "Pixel/Renderer/3D/Material/ShaderStringFactory.h"
#include "Pixel/Scene/SceneSerializer.h"
#include "Pixel/Scene/SimpleScene.h"

//------other library------
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorCpuHandle.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "rapidjson/prettywriter.h"
//------other library------

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

		m_HeaderBackgroundTexture = Texture2D::Create("Resources/Icons/BlueprintBackground.png");

		m_HeaderBackgroundTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		m_PreviewSceneTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		Device::Get()->CopyDescriptorsSimple(1, m_HeaderBackgroundTextureHandle->GetCpuHandle(), m_HeaderBackgroundTexture->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

		//copy descriptor
		Device::Get()->CopyDescriptorsSimple(1, m_PreviewSceneTextureHandle->GetCpuHandle(), pFinalColorFrameBuffer->GetColorAttachmentDescriptorCpuHandle(0), DescriptorHeapType::CBV_UAV_SRV);

		m_BlueprintNodeBuilder = CreateRef<BlueprintNodeBuilder>((ImTextureID)(m_HeaderBackgroundTextureHandle->GetGpuHandle()->GetGpuPtr()), m_HeaderBackgroundTexture->GetWidth(), m_HeaderBackgroundTexture->GetHeight());

		ShaderStringFactory::m_ShaderValueIndex = 0;

		//if it is first open, then load the settings file
		ed::Config config;

		m_GraphNodeEditorConfigPath = virtualPath + ".json";

		m_MaterialPhysicalPath = physicalPath;

		config.SettingsFile = m_GraphNodeEditorConfigPath.c_str();

		m_Editor = ed::CreateEditor(&config);

		//from the material to construct node¡¢pin¡¢link
		m_pMaterial = pMaterial;

		m_CreateNewNode = false;

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
				pGraphPins->m_PinName = m_pMaterial->GetShaderFunction()[i]->GetInputNode(j)->GetNodeName();
				pGraphPins->m_OwnerNode = pGraphNode;
				pGraphNode->m_InputPin.push_back(pGraphPins);
				m_GraphPins.push_back(pGraphPins);

				ShaderStringFactory::m_ShaderValueIndex = std::max(ShaderStringFactory::m_ShaderValueIndex, (uint32_t)pGraphPins->m_PinId.Get());
			}

			for (size_t j = 0; j < m_pMaterial->GetShaderFunction()[i]->GetOutputNodeNum(); ++j)
			{
				Ref<GraphPin> pGraphPins = CreateRef<GraphPin>();
				pGraphPins->m_PinId = m_pMaterial->GetShaderFunction()[i]->GetOutputNode(j)->GetPutNodeId();
				pGraphPins->m_PinName = m_pMaterial->GetShaderFunction()[i]->GetOutputNode(j)->GetNodeName();
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
				}
				if (m_GraphPins[i]->m_PinId.Get() == outputPinId)
				{
					pGraphLinks->m_OutputPin = m_GraphPins[i];
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
			m_TopPanelHeight = content.y / 3.0f;
			m_DownPanelHeight = content.y - m_TopPanelHeight;
		}
		//PIXEL_CORE_INFO("{0}, {1}", content.x, content.y);
		//render
		ed::SetCurrentEditor(m_Editor);
			Splitter(false, 4.0f, &m_TopPanelHeight, &m_DownPanelHeight, 10.0f, 10.0f);
			DrawTopPanel(m_TopPanelHeight + 4.0f);
			ed::Begin("Graph Node Editor Canvas");
			//draw shader main function

			//TODO:in the future, will in terms of the shading model to switch this
			DrawMainFunctionNode();

			//1.draw node
			DrawNodes();

			//2.draw link
			DrawLinks();

			//3.handle create node
			HandleInteraction();

			//4.draw preview scene
			ImGui::Image((ImTextureID)m_PreviewSceneTextureHandle->GetGpuHandle()->GetGpuPtr(), ImVec2(256, 256));

			ed::End();
		ed::SetCurrentEditor(nullptr);


		ImGui::End();
	}

	void GraphNodeEditor::OnUpdate(Timestep& ts, EditorCamera& editorCamera, Ref<Framebuffer> pGeoFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, Ref<Framebuffer> pFinalFrameBuffer)
	{
		//draw preview mesh
		m_pPreviewScene->OnUpdateEditorDeferred(ts, editorCamera, pGeoFrameBuffer, pLightFrameBuffer, pFinalFrameBuffer);
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
				m_BlueprintNodeBuilder->Header();
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
		if(ImGui::Button("Compiler", ImVec2(0, std::max(panelHeight - 8.0f, 0.0f))))
		{
			//generate and compiler

			m_pMaterial->GetMainFunction()->ClearShaderTreeStringFlag();
			//call the material's get shader tree string, then save the shader to assets/shaders/ShaderGraph
			std::string out = ShaderStringFactory::CreateDeferredGeometryShaderString(m_pMaterial);
			PIXEL_CORE_INFO(out);
		}
		if(ImGui::Button("Save", ImVec2(0, std::max(panelHeight - 8.0f, 0.0f))))
		{
			//save will also call the compiler

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
		}
		ImGui::PopStyleVar(1);
		ImGui::EndHorizontal();
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
		for(auto& node : m_GraphNodes)
		{
			//skip the main node
			if (node->m_NodeId.Get() == 1)
				continue;
			m_BlueprintNodeBuilder->Begin(node->m_NodeId);

			//draw header
			m_BlueprintNodeBuilder->Header(ImColor(255, 255, 255, 255));
			ImGui::Spring(0);
			ImGui::TextUnformatted(node->p_Owner->GetShowName().c_str());
			ImGui::Spring(1);
			ImGui::Dummy(ImVec2(0, 28));
			ImGui::Spring(0);
			m_BlueprintNodeBuilder->EndHeader();

			//draw input pin
			for(auto& input : node->m_InputPin)
			{
				m_BlueprintNodeBuilder->Input(input->m_PinId);
				DrawPinIcon(ImColor(255, 255, 255, 255), ImColor(32, 32, 32, 255));
				ImGui::Spring(0);
				ImGui::TextUnformatted(input->m_PinName.c_str());
				m_BlueprintNodeBuilder->EndInput();
			}

			//draw output pin
			for(auto& output : node->m_OutputPin)
			{
				m_BlueprintNodeBuilder->Output(output->m_PinId);
				ImGui::TextUnformatted(output->m_PinName.c_str());		
				ImGui::Spring(0);
				DrawPinIcon(ImColor(255, 255, 255, 255), ImColor(32, 32, 32, 255));
				m_BlueprintNodeBuilder->EndOutput();
			}
			m_BlueprintNodeBuilder->End();
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

				bool alreadyLink = false;
				//check the outputpin and inputpin don't have link
				for(size_t i = 0; i < m_GraphLinks.size(); ++i)
				{
					if (m_GraphLinks[i]->m_InputPin.lock() == inputPin)
						alreadyLink = true;
					if (m_GraphLinks[i]->m_OutputPin.lock() == outputPin)
						alreadyLink = true;
				}
				if(!alreadyLink)
				{
					ShowLabel("+ Create Link", ImColor(32, 45, 32, 100));
					if(ed::AcceptNewItem(ImColor(128, 255, 128, 255), 4.0f))
					{
						//create new link
						Ref<GraphLink> pGraphLink = CreateRef<GraphLink>();
						pGraphLink->m_InputPin = inputPin;
						pGraphLink->m_OutputPin = outputPin;
						pGraphLink->m_LinkId = ++ShaderStringFactory::m_ShaderValueIndex;
						//add new graph link to m_GraphLinks
						m_GraphLinks.push_back(pGraphLink);
						//create logic link
						uint32_t inputPinLocationIndex = 0, outputPinLocationIndex = 0;
						if(inputPin->m_OwnerNode.lock())
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
						if(outputPin->m_OwnerNode.lock())
						{
							for(size_t i = 0; i < outputPin->m_OwnerNode.lock()->m_OutputPin.size(); ++i)
							{
								if(outputPin->m_OwnerNode.lock()->m_OutputPin[i] == outputPin)
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
			}
			//------create link------
		}
		ed::EndCreate();

		//create new node
		CreateNewNodes();
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
			inputPin->m_PinName = pMul->GetInputNode(i)->GetNodeName();
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for(size_t i = 0; i < pMul->GetOutputNodeNum(); ++i)
		{
			pMul->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pMul->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pMul->GetOutputNode(i)->GetNodeName();
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

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
			inputPin->m_PinName = pConstFloatValue4->GetInputNode(i)->GetNodeName();
			pGraphNode->m_InputPin.push_back(inputPin);
			m_GraphPins.push_back(inputPin);
		}

		for (size_t i = 0; i < pConstFloatValue4->GetOutputNodeNum(); ++i)
		{
			pConstFloatValue4->GetOutputNode(i)->SetPutNodeId(++ShaderStringFactory::m_ShaderValueIndex);
			Ref<GraphPin> outputPin = CreateRef<GraphPin>();
			outputPin->m_PinId = pConstFloatValue4->GetOutputNode(i)->GetPutNodeId();
			outputPin->m_OwnerNode = pGraphNode;
			outputPin->m_PinName = pConstFloatValue4->GetOutputNode(i)->GetNodeName();
			pGraphNode->m_OutputPin.push_back(outputPin);
			m_GraphPins.push_back(outputPin);
		}

		m_GraphNodes.push_back(pGraphNode);

		//------add to material shader function------
		m_pMaterial->AddShaderFunction(pConstFloatValue4);
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
}
