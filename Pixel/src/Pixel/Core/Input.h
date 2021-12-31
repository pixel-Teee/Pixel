#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Core/KeyCodes.h"
#include "Pixel/Core/MouseButtonCodes.h"

namespace Pixel {

	class PIXEL_API Input
	{
	public:
		static bool IsKeyPressed(int key);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
