#include "pxpch.h"
#include "NodeGraph.h"
#include "Pixel/Renderer/3D/MaterialPass.h"

namespace Pixel {

	NodeGraph::NodeGraph()
	{
		model = Model("assets/models/Box.obj");
	}

	void NodeGraph::OnImGuiRender()
	{
		ImGui::Begin("Test Button");
		if (ImGui::Button("Test"))
		{
			MaterialPass pass;//test
			std::vector<StaticMesh> meshes = model.GetMeshes();
			Ref<StaticMesh> mesh = CreateRef<StaticMesh>(meshes[0]);
			//Ref<StaticMesh> mesh = CreateRef<StaticMesh>();
			Ref<Camera> pCamera = CreateRef<Camera>();
			Ref<Material> tempMaterial = CreateRef<Material>("Test", Material::MUT_PBR);
			Ref<MaterialInstance> pMaterialInstance = CreateRef<MaterialInstance>(tempMaterial);
			pass.SetCamera(pCamera);
			pass.SetMaterialInstance(pMaterialInstance);
			pass.SetMesh(mesh);
			pass.Draw();
		}
		ImGui::End();
	}

}