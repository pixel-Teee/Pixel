#include "ThirdPartyRegister.h"

#include <glm/glm.hpp>

namespace Pixel {
	static void rttr_auto_register_reflection_function_()
	{
		//------register------
		using namespace rttr;
		registration::class_<glm::vec3>("glm::vec3")
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);
		//------register------
	}

	void RegisterThirdPartyType()
	{
		rttr_auto_register_reflection_function_();
	}
}