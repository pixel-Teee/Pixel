#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class ShaderPbrFunction : public ShaderMainFunction
	{
	public:
		enum
		{
			IN_ALBEDO,
			IN_ROUGHNESS,
			IN_METALLIC,
			IN_NORMAL,
			IN_MAX
		};

		enum 
		{
			OUT_COLOR,
			OUT_MAX
		};

		ShaderPbrFunction();
		//two phase init
		void Init();
		virtual ~ShaderPbrFunction();
	public:

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual Ref<InputNode> GetAlbedoNode() override
		{
			return m_pInput[IN_ALBEDO];
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
