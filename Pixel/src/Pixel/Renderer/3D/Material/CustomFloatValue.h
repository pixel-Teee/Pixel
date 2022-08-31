#pragma once

#include <vector>

#include "MaterialCustomPara.h"

namespace Pixel {
	class CustomFloatValue : public MaterialCustomPara
	{
	public:
		enum class ValueType
		{
			VT_1,//float
			VT_2,//float2
			VT_3,//float3
			VT_4,//float4
			VT_4X4,//float4x4
			VT_MAX
		};

		CustomFloatValue();

		virtual ~CustomFloatValue();

		std::vector<float> m_Values;//if this value type is VT_4X4, then there will have 16 floats

		ValueType m_ValueType;

		CustomFloatValue& operator=(const CustomFloatValue& rhs)
		{
			//assignment the shader parameter name
			MaterialCustomPara::operator=(rhs);
			m_Values = rhs.m_Values;
			m_ValueType = rhs.m_ValueType;
			return *this;
		}

		friend bool operator==(const CustomFloatValue& lhs, const CustomFloatValue& rhs)
		{
			//TODO:in terms of the name?
			return lhs.ConstValueName == rhs.ConstValueName;
		}

		RTTR_ENABLE(MaterialCustomPara)
		RTTR_REGISTRATION_FRIEND
	};
}