#include "pxpch.h"

#include <yaml-cpp/yaml.h>

//------my library------
#include "LightComponent.h"
//------my library------

namespace Pixel
{
	REFLECT_STRUCT_BEGIN(LightComponent)
	REFLECT_STRUCT_MEMBER(lightType)
	REFLECT_STRUCT_MEMBER(color)
	REFLECT_STRUCT_MEMBER(GenerateShadowMap)
	REFLECT_STRUCT_MEMBER(DisplayLightVolume)
	REFLECT_STRUCT_MEMBER(constant)
	REFLECT_STRUCT_MEMBER(linear)
	REFLECT_STRUCT_MEMBER(quadratic)
	REFLECT_STRUCT_MEMBER(CutOff)
	REFLECT_STRUCT_MEMBER(MaxDistance)
	REFLECT_STRUCT_MEMBER(Range)
	REFLECT_STRUCT_MEMBER(DisplayShowdowMapFrustum)
	REFLECT_STRUCT_END()
}