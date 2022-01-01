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
	private:
		//registry is a container for entity and component
		entt::registry m_Registry;

		friend class Entity;
	};
}