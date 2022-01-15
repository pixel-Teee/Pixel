#pragma once

#include "entt.hpp"

#include "Pixel/Core/Timestep.h"
#include "Pixel/Renderer/EditorCamera.h"

class b2World;

namespace Pixel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		entt::registry& Reg() { return m_Registry; }

		void OnUpdateEditor(Timestep& ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep& ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		//registry is a container for entity and component
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

	

}