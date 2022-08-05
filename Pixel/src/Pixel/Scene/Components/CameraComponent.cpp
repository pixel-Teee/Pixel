#include "pxpch.h"

#include "CameraComponent.h"

namespace Pixel
{
	REFLECT_STRUCT_BEGIN(CameraComponent)
	REFLECT_STRUCT_MEMBER(camera)
	REFLECT_STRUCT_MEMBER(Primary)
	REFLECT_STRUCT_MEMBER(FixedAspectRatio)
	REFLECT_STRUCT_MEMBER(DisplayFurstum)
	REFLECT_STRUCT_END()
}