#include "pxpch.h"
#include "NodeGraph.h"

namespace Pixel {

	NodeGraph::NodeGraph()
	{
		m_previewBox = Model("assets/models/Box.obj");
		
		m_pStaticMesh = CreateRef<StaticMesh>(m_previewBox.GetMeshes()[0]);
		
		m_pMaterial = CreateRef<Material>("Test", Material::MUT_PBR);
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
		//------Node Graph------
	}

	NodeGraph::~NodeGraph()
	{
		ed::DestroyEditor(g_Context);
	}

	void NodeGraph::OnImGuiRender()
	{
		ImGui::Begin("Test Button");
		if (ImGui::Button("Test"))
		{
			if (m_isopen) m_isopen = false;
			else m_isopen = true;
		}
		ImGui::End();

		if (m_isopen)
		{
			ImGui::Begin("Context");
			ed::Begin("My Editor", ImVec2(0.0, 0.0f));

			m_uniqueId = 1;
			//Submit Node A
			ed::NodeId nodeA_Id = ++m_uniqueId;
			ed::PinId nodeA_InputPinId = ++m_uniqueId;
			ed::PinId nodeA_OutputPinId = ++m_uniqueId;

			if (m_isFirstFrame)
			{
				m_isFirstFrame = false;

				ed::SetNodePosition(nodeA_Id, ImVec2(10, 10));
			}

			ed::BeginNode(nodeA_Id);
				ImGui::Text("Node A");
				ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
					ImGui::Text("-> In");
				ed::EndPin();
				ImGui::SameLine();
				ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
					ImGui::Text("Out ->");
					ed::EndPin();
			ed::EndNode();
			ed::End();
			ImGui::End();
		}
	}

}