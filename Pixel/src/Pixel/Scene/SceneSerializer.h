#pragma once

#include "Scene.h"

namespace Pixel {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Writer(const std::string& filepath);

		bool Read(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}
