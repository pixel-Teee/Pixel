#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	struct BoneInfo
	{
		//id is index in finalBoneMatrices
		int32_t id;

		//offset matrix transforms vertex from model space to bone space
		glm::mat4 offset;
	};
}