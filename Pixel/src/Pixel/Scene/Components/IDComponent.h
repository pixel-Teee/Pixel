#pragma once

#include "Pixel/Core/UUID.h"
#include "Pixel/Core/Reflect.h"

namespace Pixel {
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;

		IDComponent(const IDComponent&) = default;

		REFLECT()
	};
}
