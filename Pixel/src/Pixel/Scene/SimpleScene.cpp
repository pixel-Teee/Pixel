#include "pxpch.h"

#include "SimpleScene.h"

#include "Pixel/Core/Timestep.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Scene/Components/TransformComponent.h"
#include "Pixel/Scene/Components/StaticMeshComponent.h"
#include "Pixel/Scene/Components/MaterialComponent.h"
#include "Pixel/Scene/Components/TestMaterialComponent.h"
#include "Pixel/Scene/Components/TestMaterialComponent.h"
#include "Pixel/Scene/Components/LightComponent.h"
#include "Pixel/Scene/Components/CameraComponent.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Scene/Entity.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/3D/Material/MaterialInstance.h"

namespace Pixel {
	SimpleScene::SimpleScene()
	{
		//put a cube and a direct light to scene?
		m_EntityHandle = m_Registry.create();
		m_pModel = CreateRef<Model>("Resources/models/sphere/Sphere.gltf");
		m_pSubMaterial = CreateRef<SubMaterial>();//add to material component

		m_pSubMaterial->HaveNormal = true;

		m_Registry.emplace<TransformComponent>(m_EntityHandle);
		m_Registry.emplace<StaticMeshComponent>(m_EntityHandle);
		m_Registry.get<StaticMeshComponent>(m_EntityHandle).m_Model = m_pModel;

		m_Registry.get<TransformComponent>(m_EntityHandle).Translation = glm::vec3(0.0f, 0.0f, 0.2f);
		//m_Registry.emplace<MaterialComponent>(m_EntityHandle);
		//m_Registry.get<MaterialComponent>(m_EntityHandle).m_Materials.push_back(m_pSubMaterial);

		m_LightEntityHandle = m_Registry.create();
		m_Registry.emplace<LightComponent>(m_LightEntityHandle);
		m_Registry.emplace<TransformComponent>(m_LightEntityHandle);
		m_Registry.get<LightComponent>(m_LightEntityHandle);
	}

	void SimpleScene::OnUpdateEditorDeferred(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pGeoFrameBuffer, Ref<Framebuffer>& pLightFrameBuffer, Ref<Framebuffer>& pFinalFrameBuffer, Ref<Material> pTestMaterial)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent, MaterialTreeComponent>);

		std::vector<TransformComponent*> trans;
		std::vector<StaticMeshComponent*> meshs;
		std::vector<MaterialTreeComponent*> materials;
		std::vector<int32_t> entityIds;
		for (auto entity : group)
		{
			auto& [transform, mesh, material] = group.get<TransformComponent, StaticMeshComponent, MaterialTreeComponent>(entity);

			//in terms of transform and mesh to draw
			trans.push_back(&transform);
			meshs.push_back(&mesh);
			materials.push_back(&material);
			entityIds.push_back((int32_t)entity);
		}

		std::vector<LightComponent*> lights;
		std::vector<TransformComponent*> lightTrans;
		auto lightGroup = m_Registry.group<LightComponent>(entt::get<TransformComponent>);

		std::vector<LightComponent*> SpotLights;
		std::vector<TransformComponent*> SpotLightTrans;
		std::vector<LightComponent*> DirectLights;
		std::vector<TransformComponent*> DirectLightTrans;
		for (auto entity : lightGroup)
		{
			auto& [trans, light] = lightGroup.get<TransformComponent, LightComponent>(entity);

			//in terms of transform and mesh to draw
			lightTrans.push_back(&trans);
			lights.push_back(&light);

			if (light.lightType == LightType::SpotLight)
			{
				SpotLights.push_back(&light);
				SpotLightTrans.push_back(&trans);
			}
			else if (light.lightType == LightType::DirectLight)
			{
				DirectLights.push_back(&light);
				DirectLightTrans.push_back(&trans);
			}
		}

		//important
		//Application::Get().GetRenderer()->ResetDescriptorHeapOffset();

		Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
		Application::Get().GetRenderer()->DeferredRenderingForSimpleScene(pContext, camera, trans, meshs, materials, lights, lightTrans, pGeoFrameBuffer, pLightFrameBuffer, entityIds, shared_from_this(), pTestMaterial, ts);

		pContext->Finish(true);

		//Ref<Context> pComputeContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Compute);
		//Application::Get().GetRenderer()->RenderBlurTexture(pComputeContext, pLightFrameBuffer);
		//for (uint32_t i = 0; i < 4; ++i)
		//{
		//	Application::Get().GetRenderer()->RenderBlurTexture(pComputeContext, pLightFrameBuffer);
		//}
		//pComputeContext->Finish(true);

		//Ref<Context> pPickerComputeContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Compute);
		//Application::Get().GetRenderer()->RenderPickerBuffer(pComputeContext, pGeoFrameBuffer);
		//pPickerComputeContext->Finish(true);

		Ref<Context> pFinalColorContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
		Application::Get().GetRenderer()->RenderingFinalColorBuffer(pFinalColorContext, pLightFrameBuffer, pFinalFrameBuffer);
		pFinalColorContext->Finish(true);
	}

	StaticMeshComponent& SimpleScene::GetPreviewModel()
	{
		return m_Registry.get<StaticMeshComponent>(m_EntityHandle);
	}

	void SimpleScene::SetModelMaterial(Ref<Material> pMaterial)
	{
		if (!m_Registry.any_of<MaterialTreeComponent>(m_EntityHandle))
		{
			//don't have material tree component, create a material tree component
			m_Registry.emplace<MaterialTreeComponent>(m_EntityHandle);
		}
		//------set preview model material------		
		m_Registry.get<MaterialTreeComponent>(m_EntityHandle).AddMaterialInstance();//create a new material instance
		m_Registry.get<MaterialTreeComponent>(m_EntityHandle).m_Materials[0] = CreateRef<MaterialInstance>();
		m_Registry.get<MaterialTreeComponent>(m_EntityHandle).m_Materials[0]->SetMaterial(pMaterial);
		//------set preview model material------
	}
}