#include "pxpch.h"

#include "TransformComponent.h"

#include "Pixel/Core/Reflect.h"

namespace Pixel
{
	REFLECT_STRUCT_BEGIN(TransformComponent)
	REFLECT_STRUCT_MEMBER(parentUUID)
	REFLECT_STRUCT_MEMBER(childrensUUID)
	REFLECT_STRUCT_MEMBER(Translation)
	REFLECT_STRUCT_MEMBER(Rotation)
	REFLECT_STRUCT_MEMBER(Scale)
	REFLECT_STRUCT_END()

	TransformComponent::TransformComponent(const TransformComponent& others)
	{
		parentUUID = 0;
		Translation = others.Translation;
		Rotation = others.Rotation;
		Scale = others.Scale;
		globalTransform = others.globalTransform;
	}

}