#pragma once

#include <glm/glm.hpp>

namespace Pixel
{
	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		//Editor
		int EntityID;
	};

	struct RenderObject
	{
		int BaseVertexLocation = -1;
		int StartIndexLocation = -1;
		int EndIndexLocation = -1;
	};
}