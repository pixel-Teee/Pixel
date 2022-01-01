#pragma once

#include "entt.hpp"

#include "Pixel/Core/Timestep.h"

namespace Pixel
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();
		
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep& ts);
	private:
		//registry is a container for entity and component
		entt::registry m_Registry;
	};
}