#pragma once

#include <vector>

#include "PutNode.h"//TODO:use value type, need to fix this, remove this
#include "MaterialCustomPara.h"

namespace Pixel {

	class Meta(Enable) CustomFloatValue : public MaterialCustomPara
	{
	public:
		CustomFloatValue();

		virtual ~CustomFloatValue();

		Meta()
		std::vector<float> m_Values;//if this value type is VT_4X4, then there will have 16 floats

		Meta()
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

		friend class Material;
	};
}