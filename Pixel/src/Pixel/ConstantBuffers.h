#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	struct alignas(256) GlobalConstants
	{
		glm::mat4x4 gView;
		glm::mat4x4 gProjection;
		glm::mat4x4 gViewProjection;
		glm::vec3 cameraPosition;
		float padding0;
		glm::vec3 LightPosition;
		float padding1;
		glm::vec3 LightColor;
	};
}