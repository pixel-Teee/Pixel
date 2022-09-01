#include "pxpch.h"

#include "CustomFloatValue.h"

namespace Pixel {


}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::CustomFloatValue>("CustomFloatValue")
		.constructor<>()
		.property("m_Values", &Pixel::CustomFloatValue::m_Values)
		.property("m_ValueType", &Pixel::CustomFloatValue::m_ValueType);
}