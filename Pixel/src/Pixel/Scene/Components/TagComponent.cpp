#include "pxpch.h"

#include "TagComponent.h"

namespace Pixel
{


}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::TagComponent>("TagComponent")
		.constructor<>()
		.property("Tag", &Pixel::TagComponent::Tag);

	std::cout << "register tag component successfully!" << std::endl;
}