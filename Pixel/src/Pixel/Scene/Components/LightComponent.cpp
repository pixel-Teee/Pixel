#include "pxpch.h"

#include <yaml-cpp/yaml.h>

//------my library------
#include "LightComponent.h"
//------my library------

namespace Pixel
{

}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::LightComponent>("LightComponent")
		.constructor<>()
		.property("lightType", &Pixel::LightComponent::lightType)
		.property("color", &Pixel::LightComponent::color)
		.property("GenerateShadowMap", &Pixel::LightComponent::GenerateShadowMap)
		.property("DisplayLightVolume", &Pixel::LightComponent::DisplayLightVolume)
		.property("constant", &Pixel::LightComponent::constant)
		.property("linear", &Pixel::LightComponent::linear)
		.property("quadratic", &Pixel::LightComponent::quadratic)
		.property("CutOff", &Pixel::LightComponent::CutOff)
		.property("MaxDistance", &Pixel::LightComponent::MaxDistance)
		.property("Range", &Pixel::LightComponent::Range)
		.property("DisplayShowdowMapFrustum", &Pixel::LightComponent::DisplayShowdowMapFrustum);

	//PIXEL_CORE_INFO("register successfully!");
	std::cout << "register light component successfully!" << std::endl;
}