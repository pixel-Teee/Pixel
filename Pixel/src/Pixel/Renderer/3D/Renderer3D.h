#pragma once

#include "Pixel/Scene/Components.h"

namespace Pixel{

	class Renderer3D
	{
	public:
		static void Init();

		static void DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, int EntityID);

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();
	};

}