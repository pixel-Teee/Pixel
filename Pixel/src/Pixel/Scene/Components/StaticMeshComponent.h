#pragma once

#include "Pixel/Renderer/3D/Model.h"

namespace Pixel {
	struct StaticMeshComponent
	{
		Model mesh;
		//std::string path;

		//char path[256];
		std::string path;

		//Editor's Attribute
		int currentItem;

		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
		StaticMeshComponent(const std::string& Path)
		{
			//memcpy(path, Path.c_str(), sizeof(char) * Path.size());
			//path[Path.size()] = '\0';
			path = Path;
		}

		void PostLoad()
		{
			mesh = Model(path);
		}

		REFLECT()
	};
}