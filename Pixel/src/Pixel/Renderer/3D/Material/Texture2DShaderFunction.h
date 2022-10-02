#pragma once

#include "TextureShaderFunction.h"

namespace Pixel {

	class Meta(Enable) Texture2DShaderFunction : public TextureShaderFunction
	{
	public:

		Meta()
		Texture2DShaderFunction();

		Texture2DShaderFunction(const std::string & ShowName, Ref<Material> pMaterial);

		virtual void ConstructPutNodeAndSetPutNodeOwner() override;

		virtual ~Texture2DShaderFunction();

		virtual bool GetFunctionString(std::string & OutString) const;

		virtual void ResetInShaderName();

		virtual bool GetOutputValueString(std::string & OutString) const;

		enum InputValueElement
		{
			IN_TEXCOORD,
			IN_MAX
		};

		enum OutputValueElement
		{
			OUT_COLOR,
			OUT_COLOR_R,
			OUT_COLOR_G,
			OUT_COLOR_B,
			OUT_COLOR_A,
			OUT_MAX
		};

		RTTR_ENABLE(TextureShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}