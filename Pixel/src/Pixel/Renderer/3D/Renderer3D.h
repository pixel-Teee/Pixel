#pragma once

#include "Pixel/Scene/Components.h"

namespace Pixel{

	class Renderer3D
	{
	public:
		static void Init();

		static void DrawQube(const glm::mat4& transform);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();
		static void Flush();
	};

}