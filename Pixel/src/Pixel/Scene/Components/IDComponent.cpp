#include "pxpch.h"

#include "IDComponent.h"

namespace Pixel
{
	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<IDComponent>("IDComponent")
			.constructor<>()
			.property("ID", &IDComponent::ID);
	}
}