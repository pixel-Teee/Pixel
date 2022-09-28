#pragma once

#include "entt.hpp"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"

namespace Pixel {
	class Model;
	class Material;
	struct TransformComponent;
	struct StaticMeshComponent;
	struct SubMaterial;
	struct MaterialComponent;
	//just for graph node editor's preview
	class SimpleScene : public std::enable_shared_from_this<SimpleScene>
	{
	public:
		SimpleScene();
		
		void OnUpdateEditorDeferred(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pGeoFrameBuffer, Ref<Framebuffer>& pLightFrameBuffer, Ref<Framebuffer>& pFinalFrameBuffer, Ref<Material> pTestMaterial);

		entt::registry& GetRegistry() { return m_Registry; }

		StaticMeshComponent& GetPreviewModel();
	private:
		entt::registry m_Registry;

		//preview model
		Ref<Model> m_pModel;

		//Ref<TransformComponent> m_pTransformComponent;

		//Ref<StaticMeshComponent> m_pMeshComponent;

		Ref<SubMaterial> m_pSubMaterial;
		//preview material component
		//Ref<MaterialComponent> m_pMaterialComponent;

		//preview entity
		entt::entity m_EntityHandle{ entt::null };

		entt::entity m_LightEntityHandle{ entt::null };
	};
}