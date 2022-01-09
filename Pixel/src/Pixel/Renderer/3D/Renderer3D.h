#pragma once

#include "Pixel/Renderer/Renderer.h"

namespace Pixel{

	class Renderer3D
	{
	public:
		static void Init();

		static void DrawQube();

		static void BeginScene();
		static void EndScene();
		static void Flush();
	};

}