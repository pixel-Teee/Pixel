#pragma once

//------Cpp library------
#include <vector>
//------Cpp library------

//------others library------
#include "entt.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//------others library------

//------my library------
#include "Pixel/Core/UUID.h"
#include "Pixel/Scene/Components/IDComponent.h"
//------my library------

namespace Pixel {
	struct TransformComponent
	{
		UUID parentUUID = 0;//parent uuid
		std::vector<UUID> childrensUUID;//children uuid
		//------local transform------
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		//------local transform------
		glm::mat4 globalTransform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&);
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}

		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetForwardDirection()
		{
			//just for light direction
			return glm::rotate(glm::quat(Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glm::mat4 GetGlobalTransform(entt::registry& scene) const
		{
			if (parentUUID != 0)
			{
				//parent entity
				entt::entity parentEntity;
				//find uuid's owner
				auto& UUIDs = scene.view<IDComponent>();
				for (auto entity : UUIDs)
				{
					const IDComponent& id = scene.get<IDComponent>(entity);
					if (id.ID == parentUUID)
					{
						parentEntity = entity;
						break;
					}
				}

				TransformComponent parentTransformComponent = scene.get<TransformComponent>(parentEntity);
				return GetLocalTransform() * parentTransformComponent.GetGlobalTransform(scene);//additive
			}
			else
			{
				return GetLocalTransform();
			}
		}

		glm::mat4 GetGlobalParentTransform(entt::registry& scene) const
		{
			if (parentUUID != 0)
			{
				//parent entity
				entt::entity parentEntity;
				//find uuid's owner
				auto& UUIDs = scene.view<IDComponent>();
				for (auto entity : UUIDs)
				{
					const IDComponent& id = scene.get<IDComponent>(entity);
					if (id.ID == parentUUID)
					{
						parentEntity = entity;
						break;
					}
				}

				TransformComponent parentTransformComponent = scene.get<TransformComponent>(parentEntity);
				return parentTransformComponent.GetGlobalTransform(scene);
			}
			else
			{
				return glm::mat4(1.0f);
			}
		}

		void SetScale(glm::vec3 scale)
		{
			Scale = scale;
		}

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}