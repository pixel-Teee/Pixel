#pragma once

#include "Pixel/Renderer/Shader.h"

namespace Pixel {
	class PutNode;
	struct MaterialShaderPara;
	class Material;
	class StaticMesh;
	class Texture2DShaderFunction;
	class ShaderStringFactory
	{
	public:
		//------extract the value element of putnode, ect xxx.x------
		enum ValueElement
		{
			VE_NONE = 0,
			VE_R = 1,
			VE_G = BIT(1),
			VE_B = BIT(2),
			VE_A = BIT(3),
			DF_ALL = 0x0F
		};
		//------extract the value element of putnode, ect xxx.x------

		//give the temp variable one id, variable name + id
		static uint32_t m_ShaderValueIndex;

		static void Init();

		static std::string Float();

		static std::string Float2();

		static std::string Float3();

		static std::string Float4();

		static std::string Return();

		static std::string FloatConst(const std::string& value);

		static std::string FloatConst2(const std::string& value1, const std::string& value2);

		static std::string FloatConst3(const std::string& value1, const std::string& value2, const std::string& value3);

		static std::string FloatConst4(const std::string& value1, const std::string& value2, const std::string& value3, const std::string& value4);

		static std::string Tex2D(Texture2DShaderFunction* pTexture2DShaderFunction);

		static std::string GetValueElement(Ref<PutNode> pPutNode, ValueElement valueElement);

		static std::string CreateDeferredGeometryShaderString(Ref<Material> pMaterial, Ref<StaticMesh> pStaticMesh);

		static void GetIncludeShader(std::string& Out);

		static void CreatePixelShaderUserConstant(std::string& Out, Ref<Material> pMaterial);
	};
}