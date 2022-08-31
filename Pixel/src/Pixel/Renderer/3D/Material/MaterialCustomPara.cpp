#include "pxpch.h"

#include "MaterialCustomPara.h"

namespace Pixel {

	MaterialCustomPara::~MaterialCustomPara()
	{

	}

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<MaterialCustomPara>("MaterialCustomPara")
			.constructor<void>()
			.property("ConstValueName", &MaterialCustomPara::ConstValueName);
	}
}