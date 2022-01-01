#include "pxpch.h"

#include "Entity.h"

namespace Pixel {


	Entity::Entity(entt::entity handle, Scene* scene)
	:m_EntityHandle(handle), m_Scene(scene)
	{

	}
}