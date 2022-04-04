#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class ShaderGeometryFunction : public ShaderMainFunction
	{
	public:
		enum
		{
			IN_POS,
			IN_ALBEDO,
			IN_NORMAL,
			IN_ROUGHNESS,
			IN_METALLIC,
			IN_EMISSIVE,
			IN_MAX
		};

		enum 
		{
			OUT_POS,
			OUT_ALBEDO,
			OUT_NORMAL,
			OUT_ROUGHNESS,
			OUT_METALLIC,
			OUT_EMISSIVE,
			OUT_MAX
		};

		ShaderGeometryFunction();
		//two phase init
		void Init();
		virtual ~ShaderGeometryFunction();
	public:

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual Ref<InputNode> GetWorldPosNode() override
		{
			return m_pInput[IN_POS];
		}

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

		virtual Ref<InputNode> GetEmissiveNode() override
		{
			return m_pInput[IN_EMISSIVE];
		}

		virtual Ref<InputNode> GetNormalNode() override
		{
			return m_pInput[IN_NORMAL];
		}
	};
}
