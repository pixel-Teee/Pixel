#pragma once

#include "entt.hpp"

namespace Pixel {
	class Framebuffer;
	class Material;
	class Model;
	class EditorCamera;
	class Timestep;
	class ThumbNailScene : public std::enable_shared_from_this<ThumbNailScene>
	{
	public:
		ThumbNailScene();

		void OnUpdateEditorDeferred(EditorCamera& editorCamera, Ref<Framebuffer>& pGeomFrameBuffer, Ref<Framebuffer>& pLightFrameBuffer, Ref<Framebuffer>& pFinalFrameBuffer,
			Ref<Material> pTestMaterial, Timestep& ts);

		entt::registry& GetRegistry() { return m_Registry; }

		void SetModelMaterial(Ref<Material> pMaterial);
	private:
		entt::registry m_Registry;

		Ref<Model> m_pModel;

		entt::entity m_EntityHandle{ entt::null };

		entt::entity m_LightEntityHandle{ entt::null };
	};
}