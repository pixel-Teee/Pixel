#pragma once

#include "entt.hpp"

#include "Pixel/Core/Timestep.h"

namespace Pixel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name);
		
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep& ts);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		//registry is a container for entity and component
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}