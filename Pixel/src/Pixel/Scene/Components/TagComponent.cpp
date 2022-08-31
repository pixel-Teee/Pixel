#include "pxpch.h"

#include "TagComponent.h"

namespace Pixel
{

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<TagComponent>("TagComponent")
			.constructor<>()
			.property("Tag", &TagComponent::Tag);
	}
}