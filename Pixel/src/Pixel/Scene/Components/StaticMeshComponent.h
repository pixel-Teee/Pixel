#pragma once

namespace Pixel {
	class Model;
	struct Meta(Enable) StaticMeshComponent
	{
		Meta()
		Ref<Model> m_Model;
		//std::string path;

		//char path[256];
		Meta()
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

		void PostLoad();

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}