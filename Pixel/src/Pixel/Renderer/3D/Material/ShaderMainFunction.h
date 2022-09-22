#pragma once

#include "ShaderFunction.h"

namespace Pixel
{

	class Meta(Enable) ShaderMainFunction : public ShaderFunction
	{
	public:
		enum PutNodeType
		{
			IN_NORMAL = 0,
			IN_ALBEDO = 1,
			IN_ROUGHNESS = 2,
			IN_METALLIC = 3,
			IN_AO = 4
		};

		Meta()
		ShaderMainFunction();

		virtual bool GetShaderTreeString(std::string& OutString) override;

		virtual Ref<InputNode> GetNormalNode();

		virtual void ConstructPutNodeAndSetPutNodeOwner() override;

		void GetNormalString(std::string& OutString);

		virtual bool GetFunctionString(std::string& OutString) const override;

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}