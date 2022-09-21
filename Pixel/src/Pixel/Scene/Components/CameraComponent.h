#pragma once

#include "Pixel/Scene/SceneCamera.h"

namespace Pixel {

	struct Meta(Enable) CameraComponent
	{
		Meta()
		SceneCamera camera;
		//think about move to scene
		Meta()
		bool Primary = true;
		Meta()
		//when OnResize, whether is SetViewport
		bool FixedAspectRatio = false;

		//draw frustum
		//editor only
		Meta()
		bool DisplayFurstum = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4& projection);

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}