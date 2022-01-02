#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

namespace Pixel {
	
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
		:Tag(tag){}
	};

	struct TransformComponent
	{
		glm::mat4 Transform{1.0f};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : Transform(transform){}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; };
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; };
	};

	struct CameraComponent
	{
		SceneCamera camera;
		//think about move to scene
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4& projection);
	};

	struct NativeComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;
		std::function<void(ScriptableEntity* instance)> OnCreateFunction;
		std::function<void(ScriptableEntity* instance)> OnDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;

		template<typename T>
		void Bind()
		{
			InstantiateFunction = [&](){ Instance = new T();};
			DestroyInstanceFunction = [&]() { delete (T*)Instance; Instance = nullptr; };

			OnCreateFunction = [&](ScriptableEntity* instance){ ((T*)instance)->OnCreate(); };
			OnDestroyFunction = [&](ScriptableEntity* instance){ ((T*)instance)->OnDestroy(); };
			OnUpdateFunction = [&](ScriptableEntity* instance, Timestep ts){ ((T*)instance)->OnUpdate(ts); };
		}
	};
}
