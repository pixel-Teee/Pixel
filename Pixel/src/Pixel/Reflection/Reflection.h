#include <iostream>

#include "Pixel/Core/Core.h"

#include <rttr/type>
#include <rttr/registration_friend>

#if defined(__REFLECTION_PARSER__)
#   define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#   define Meta(...)
#endif

#include "Pixel/Scene/Components/TagComponent.h"
#include "Pixel/Scene/Components/IDComponent.h"
#include "Pixel/Scene/Components/LightComponent.h"
#include "Pixel/Scene/Components/CameraComponent.h"
#include "Pixel/Scene/Components/StaticMeshComponent.h"
#include "Pixel/Scene/Components/MaterialComponent.h"
#include "Pixel/Scene/Components/TestMaterialComponent.h"

#include "Pixel/Core/UUID.h"
#include "Pixel/Scene/SceneCamera.h"
#include "Pixel/Renderer/3D/Material/Material.h"
#include "Pixel/Renderer/3D/Material/ShaderFunction.h"