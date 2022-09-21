#include "pxpch.h"

#include "TransformComponent.h"

#include "Pixel/Core/Reflect.h"

namespace Pixel
{

	TransformComponent::TransformComponent(const TransformComponent& others)
	{
		parentUUID = 0;
		Translation = others.Translation;
		Rotation = others.Rotation;
		Scale = others.Scale;
		globalTransform = others.globalTransform;
	}
}