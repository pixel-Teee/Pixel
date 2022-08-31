#include "pxpch.h"

#include "CameraComponent.h"

namespace Pixel
{
	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<CameraComponent>("CameraComponent")
			.constructor<>()
			.property("camera", &CameraComponent::camera)
			.property("Primary", &CameraComponent::Primary)
			.property("FixedAspectRatio", &CameraComponent::FixedAspectRatio)
			.property("DisplayFurstum", &CameraComponent::DisplayFurstum);
	}
}