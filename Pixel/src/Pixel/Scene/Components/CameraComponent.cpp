#include "pxpch.h"

#include "CameraComponent.h"

namespace Pixel
{
	
}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::CameraComponent>("CameraComponent")
		.constructor<>()
		.property("camera", &Pixel::CameraComponent::camera)
		.property("Primary", &Pixel::CameraComponent::Primary)
		.property("FixedAspectRatio", &Pixel::CameraComponent::FixedAspectRatio)
		.property("DisplayFurstum", &Pixel::CameraComponent::DisplayFurstum);

	std::cout << "register camera component successfully!" << std::endl;
}