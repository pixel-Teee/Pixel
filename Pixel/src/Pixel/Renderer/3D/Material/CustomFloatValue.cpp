#include "pxpch.h"

#include "CustomFloatValue.h"

namespace Pixel {

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<CustomFloatValue>("CustomFloatValue")
			.constructor<void>()
			.property("m_Values", &CustomFloatValue::m_Values)
			.property("m_ValueType", &CustomFloatValue::m_ValueType);
	}
}