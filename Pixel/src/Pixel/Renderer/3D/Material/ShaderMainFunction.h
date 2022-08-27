#pragma once

#include "ShaderFunction.h"

namespace Pixel
{
	class ShaderMainFunction : public ShaderFunction
	{
	public:
		enum PutNodeType
		{
			IN_NORMAL = 0,
			IN_ALBEDO = 1
		};
		virtual bool GetShaderTreeString(std::string& OutString) override;

		virtual Ref<InputNode> GetNormalNode();

		virtual void ConstructPutNodeAndSetPutNodeOwner() override;

		void GetNormalString(std::string& OutString);

		virtual bool GetFunctionString(std::string& OutString) const override;
	};
}