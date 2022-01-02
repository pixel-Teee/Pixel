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

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
}
