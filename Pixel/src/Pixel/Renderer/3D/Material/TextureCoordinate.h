#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) TextureCoordinate : public ShaderFunction
	{
	public:
		//Meta()
		TextureCoordinate();
		//if bIsCustom, then will declare as uniform variable
		TextureCoordinate(const std::string & showName, Ref<Material> pMaterial);

		virtual bool GetFunctionString(std::string& OutString) const;//get function string

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		virtual void ResetInShaderName();

		//virtual bool GetDeclareString(std::string & OutString, int32_t registerId = -1) const = 0;

		virtual ~TextureCoordinate();

		enum OutputValueElement
		{
			OUT_COLOR,
			OUT_MAX
		};

		/*
		enum TexCoordIndex //texcoordinate index
		{
			TI_1,
			TI_2,
			TI_MAX
		};
		*/

		//temporarily use two texcoordinate value index
		//void SetTexCoordIndex(uint32_t uiIndex);

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}