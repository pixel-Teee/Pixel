#pragma once

#include "Pixel/Scene/SceneCamera.h"

namespace Pixel {

	struct CameraComponent
	{
		SceneCamera camera;
		//think about move to scene
		bool Primary = true;
		//when OnResize, whether is SetViewport
		bool FixedAspectRatio = false;

		//draw frustum
		//editor only
		bool DisplayFurstum = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4& projection);

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}