#pragma once

#include "Pixel/Core/UUID.h"
#include "Pixel/Core/Reflect.h"

namespace Pixel {
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;

		IDComponent(UUID id) { ID = id; }

		IDComponent(const IDComponent&) = default;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}
