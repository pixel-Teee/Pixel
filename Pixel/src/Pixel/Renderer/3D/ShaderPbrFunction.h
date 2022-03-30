#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class ShaderPbrFunction : public ShaderMainFunction
	{
	public:
		enum
		{
			IN__ALBEDO,
			IN_ROUGHNESS,
			IN_METALLIC,
			IN_NORMAL,
			IN_MAX
		};
		ShaderPbrFunction();
		virtual ~ShaderPbrFunction();
	public:
		virtual bool IsHaveOutput() override;


		virtual bool GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType) override;


		virtual bool GetOutputValueString(std::string& OutString) const override;


		virtual bool GetFunctionString(std::string& OutString) const override;


		virtual bool GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType) override;


		virtual bool GetInputValueString(std::string& OutString) const override;

		virtual Ref<InputNode> GetAlbedoNode() override
		{
			return m_pInput[IN__ALBEDO];
		}

		virtual Ref<InputNode> GetRoughness() override
		{
			return m_pInput[IN_ROUGHNESS];
		}

		virtual Ref<InputNode> GetMetallicNode() override
		{
			return m_pInput[IN_METALLIC];
		}

		virtual Ref<InputNode> GetNormalNode() override
		{
			return m_pInput[IN_NORMAL];
		}
	};
}
