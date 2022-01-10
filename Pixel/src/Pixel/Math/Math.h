#pragma  once

#include <glm/glm.hpp>

namespace Pixel {

	namespace Math {
		bool DecomposeTransform(const glm::mat4& tranform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	}
}