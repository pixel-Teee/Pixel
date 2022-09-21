#pragma once

#include "Pixel/Core/UUID.h"

namespace Pixel {
	struct Meta(Enable) IDComponent
	{
		Meta()
		UUID ID;

		IDComponent() = default;

		IDComponent(UUID id) { ID = id; }

		IDComponent(const IDComponent&) = default;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}
