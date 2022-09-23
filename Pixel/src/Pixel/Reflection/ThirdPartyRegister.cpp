#include "ThirdPartyRegister.h"

#include <glm/glm.hpp>

namespace Pixel {
	static void rttr_auto_register_reflection_function_()
	{
		//------register------
		using namespace rttr;
		registration::class_<glm::vec3>("glm::vec3")
		.constructor()
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);
		//------register------

		registration::class_<glm::vec2>("glm::vec2")
		.constructor()
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y);

		registration::class_<glm::ivec2>("glm::ivec2")
		.constructor()
		.property("x", &glm::ivec2::x)
		.property("y", &glm::ivec2::y);
	}

	void RegisterThirdPartyType()
	{
		rttr_auto_register_reflection_function_();
	}
}