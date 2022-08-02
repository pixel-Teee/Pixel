#include "pxpch.h"

#include "LightComponent.h"

namespace Pixel {
	REFLECT_STRUCT_BEGIN(LightComponent)
	REFLECT_STRUCT_MEMBER(color)
	REFLECT_STRUCT_MEMBER(GenerateShadowMap)
	REFLECT_STRUCT_MEMBER(DisplayLightVolume)
	REFLECT_STRUCT_MEMBER(constant)
	REFLECT_STRUCT_MEMBER(linear)
	REFLECT_STRUCT_MEMBER(quadratic)
	REFLECT_STRUCT_END()
}