#include "pxpch.h"

#include <yaml-cpp/yaml.h>

//------my library------
#include "LightComponent.h"
//------my library------

namespace Pixel
{
	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<LightComponent>("LightComponent")
			.constructor<>()
			.property("lightType", &LightComponent::lightType)
			.property("color", &LightComponent::color)
			.property("GenerateShadowMap", &LightComponent::GenerateShadowMap)
			.property("DisplayLightVolume", &LightComponent::DisplayLightVolume)
			.property("constant", &LightComponent::constant)
			.property("linear", &LightComponent::linear)
			.property("quadratic", &LightComponent::quadratic)
			.property("CutOff", &LightComponent::CutOff)
			.property("MaxDistance", &LightComponent::MaxDistance)
			.property("Range", &LightComponent::Range)
			.property("DisplayShowdowMapFrustum", &LightComponent::DisplayShowdowMapFrustum);
	}
}