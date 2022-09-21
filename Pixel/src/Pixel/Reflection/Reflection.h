#include <iostream>

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