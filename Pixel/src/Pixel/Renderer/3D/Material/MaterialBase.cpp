#include "pxpch.h"

#include "MaterialBase.h"

namespace Pixel {

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<MaterialBase>("MaterialBase")
			.constructor<>()
			.property("m_PSShaderCustomValue", &MaterialBase::m_PSShaderCustomValue);
	}
}