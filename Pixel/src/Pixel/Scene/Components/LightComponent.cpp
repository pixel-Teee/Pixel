#include "pxpch.h"

#include "LightComponent.h"

namespace Pixel {
	REFLECT_STRUCT_BEGIN(LightComponent)
	REFLECT_STRUCT_MEMBER(color)
	REFLECT_STRUCT_MEMBER(GenerateShadowMap)
	REFLECT_STRUCT_MEMBER(DisplayLightVolume)
	REFLECT_STRUCT_END()
}