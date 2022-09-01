#include "pxpch.h"

#include "IDComponent.h"

namespace Pixel
{

}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::IDComponent>("IDComponent")
		.constructor<>()
		.property("ID", &Pixel::IDComponent::ID);

	std::cout << "register id component successfully!" << std::endl;
}
