#pragma once

//------other library------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//------other library------

//------my library------
#include <Pixel/Scene/Components/LightComponent.h>
//------my library------


namespace Pixel {
	enum class LightType
	{
		PointLight = 0,
		DirectLight = 1,
		SpotLight = 2
	};

	//TODO£ºtemporary there only have a point light
	struct LightComponent
	{
		LightType lightType;
		//LightColor
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		bool GenerateShadowMap = false;//temporarily for direct light
		bool DisplayLightVolume = false;//temporarily for point light
		//------for point light's volume radius------
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		//------for point light's volume radius------

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;

		float GetSphereLightVolumeRadius()
		{
			float lightMax = glm::compMax(color);

			//light volume sphere radius
			float radius =
				(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * lightMax))) / (2 * quadratic);

			return radius;
		}

		glm::mat4 GetTransformComponent(TransformComponent transformComponent)
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent.Rotation));

			return glm::translate(glm::mat4(1.0f), transformComponent.Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), transformComponent.Scale * GetSphereLightVolumeRadius());
		}
	};
}