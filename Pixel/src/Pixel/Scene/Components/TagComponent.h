#pragma once

#include "Pixel/Core/Reflect.h"

namespace Pixel {
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}