#pragma once

//------other library------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//------other library------

namespace Pixel {
	struct RigidBody3DComponent
	{
		enum ShapeType {
			BoxShape,
			SphereShape,
			ConvexHull
		};
		ShapeType m_shapeType;
		//fix:complete type
		void* m_body = nullptr;
		void* m_shape = nullptr;

		float m_bodyMass;
		glm::vec3 m_bodyInertia;
		float m_restitution = 1.0f;
		float m_friction = 0.0f;

		float m_Radius;
		float m_HalfLength;

		RigidBody3DComponent() = default;
		RigidBody3DComponent(const RigidBody3DComponent& rhs)
		{
			//copy pointer???
			//need to fix this function
			m_bodyMass = rhs.m_bodyMass;
			m_bodyInertia = rhs.m_bodyInertia;
			m_restitution = rhs.m_restitution;
			m_friction = rhs.m_friction;

			m_Radius = rhs.m_Radius;
			m_HalfLength = rhs.m_HalfLength;
		}
	};
}