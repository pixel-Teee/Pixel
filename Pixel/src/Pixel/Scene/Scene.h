#pragma once

#include "entt.hpp"
#include "Pixel/Core/UUID.h"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"
#include "Pixel/Renderer/Texture.h"

class b2World;

namespace Pixel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		entt::registry& Reg() { return m_Registry; }

		void OnUpdateEditor(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer);
		void OnUpdateRuntime(Timestep& ts, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		void SetViewPortSize(int m_ViewportWidth, int m_ViewportHeight);
		glm::vec2 GetViewPortSize();

		void SetSkyBox(Ref<CubeMap> skyBox);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		//environment
		Ref<CubeMap> m_skyBox;

		//registry is a container for entity and component
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

	

}