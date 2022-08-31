#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <thread>

#include "Pixel/Core/Log.h"
#include "Pixel/Debug/Instrumentor.h"
#include "Pixel/Utils/Hash.h"
#include <rttr/type>
#include <rttr/registration>
#include <rttr/registration_friend.h>
#include <rttr/detail/constructor/constructor_invoker.h>
#include <rttr/detail/base/core_prerequisites.h>

#ifdef PX_PLATFORM_WINDOWS
	#include <Windows.h>
	#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
	#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS)-1)
#endif

