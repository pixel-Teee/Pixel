#pragma once

#include "ConstValue.h"

namespace Pixel
{
	class ConstFloatValue : public ConstValue
	{
	public:
		ConstFloatValue() = default;
		ConstFloatValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom);

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		//set param value
		void SetValue(uint32_t index, float value);
		//similar to vec4 NodeNameID.Type
		virtual bool GetOutputValueString(std::string& OutString) const;
		//in terms of whether is user's uniform variable
		virtual bool GetFunctionString(std::string& OutString) const;
		//if ConstFloatValue is uniform, then registerId will use, but glsl don't have this
		//else don't use registerId
		bool GetDeclareString(std::string& OutString, uint32_t registerId) const;
		//before compile shader, will call this function reset temp variable name
		virtual void ResetInShaderName();

		std::vector<float> m_Value;

		//value number 
		uint32_t m_valueNumber;

		enum OutValueElement
		{
			OUT_VALUE,
			OUT_VALUE_X,
			OUT_VALUE_Y,
			OUT_VALUE_Z,
			OUT_VALUE_w,
			OUT_MAX
		};

		REFLECT()
	};
}