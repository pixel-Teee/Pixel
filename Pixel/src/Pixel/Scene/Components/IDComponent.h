#pragma once

namespace Pixel {
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;

		IDComponent(const IDComponent&) = default;
	};
}