#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Camera.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/3D/StaticMesh.h"
#include "Pixel/Renderer/3D/ShaderFunction.h"

#include <vector>
#include <string>

namespace Pixel {
	class MaterialInterface {
	public:
		virtual ~MaterialInterface() = default;

		static void Create();
	};

	//------Param------
	class MaterialCustomPara
	{
	public:
		MaterialCustomPara() = default;
		virtual ~MaterialCustomPara();
		//shader parameter name
		std::string ConstValueName;
		
		MaterialCustomPara& operator=(const MaterialCustomPara& Para)
		{
			ConstValueName = Para.ConstValueName;
			return *this;
		}

		friend bool operator>(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName > Para2.ConstValueName;
		}

		friend bool operator<(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName < Para2.ConstValueName;
		}

		friend bool operator==(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName == Para2.ConstValueName;
		}
	};

	class CustomFloatValue : public MaterialCustomPara
	{
	public:
		enum ValueType
		{
			VT_1,//float
			VT_2,//vec1
			VT_3,//vec2
			VT_4,//vec4
			VT_4x4,//Matrix4x4
			VT_MAX
		};
		CustomFloatValue();
		virtual ~CustomFloatValue();
		std::vector<float> Value;
		ValueType m_valueType;
		CustomFloatValue& operator=(const CustomFloatValue& Para)
		{
			//assignment
			MaterialCustomPara::operator=(Para);
			Value = Para.Value;
			return *this;
		}
		friend bool operator==(const CustomFloatValue& Para1, const CustomFloatValue& Para2)
		{
			if (Para1.ConstValueName == Para2.ConstValueName) return true;
		}
	};

	class CustomTexSampler : public MaterialCustomPara
	{
	public:
		CustomTexSampler();
		virtual ~CustomTexSampler();
		//is pointer
		Ref<Texture2D> m_pTexture;
		CustomTexSampler& operator=(const CustomTexSampler& Para)
		{
			MaterialCustomPara::operator =(Para);
			m_pTexture = Para.m_pTexture;
			return *this;
		}
		friend bool operator==(const CustomTexSampler& Para1, const CustomTexSampler& Para2)
		{
			if (Para1.ConstValueName == Para2.ConstValueName) return true;
		}
	};
	//------Param------

	struct MaterialShaderPara;
	class MaterialBase : public MaterialInterface {
	public:
		//Vertex Shader
		std::vector<Ref<CustomFloatValue>> m_VSShaderCustomValue;
		std::vector<Ref<CustomTexSampler>> m_VSShaderCustomTex;
		//Pixel Shader
		std::vector<Ref<CustomFloatValue>> m_PSShaderCustomValue;
		std::vector<Ref<CustomTexSampler>> m_PSShaderCustomTex;
	};

	class Material : public MaterialBase
	{
	//TODO:protected 
	public:
		Material();
		virtual ~Material();
		Material(const std::string& showName, uint32_t uiMUT = MUT_PBR);
		//material name
		std::string m_ShowName;
		//logic node
		//main light calculate shader
		std::vector<Ref<ShaderMainFunction>> m_pShaderMainFunction;
		//other node
		std::vector<Ref<ShaderFunction>> m_pShaderFunctionArray;

	public:
		enum {
			MUT_PBR,
			MUT_LIGHT,
			MUT_MAX
		};
		Ref<ShaderMainFunction> GetMainFunction(uint32_t uiPassId);
		std::vector<Ref<ShaderFunction>> GetShaderFunction();
		void AddShaderFunction(Ref<ShaderFunction> pShaderFunction);
		void DeleteShaderFunction(Ref<ShaderFunction> pShaderFunction);
		bool GetShaderTreeString(std::string& OutString, MaterialShaderPara& MSPara, uint32_t uiOST, uint32_t uiPassId);
	};

	//multiple MaterialInstance can shader one Material
	class MaterialInstance
	{
	public:
		MaterialInstance(Ref<Material> pMaterial);
		virtual ~MaterialInstance();
	protected:
		MaterialInstance();

		//param
		std::vector<CustomFloatValue> m_VSShaderCustomValue;
		std::vector<CustomFloatValue> m_PSShaderCustomValue;
		std::vector<CustomTexSampler> m_VSShaderCustomTex;
		std::vector<CustomTexSampler> m_PSShaderCustomTex;
		//pointer to material
		Ref<Material> m_pMaterial;
	public:
		//Current Shader, will be set from pass
		Ref<Shader> m_CurrShader;
		//set shader param, total
		void SetPSShaderValue(Ref<Shader> pPSShader);
		void SetVSShaderValue(Ref<Shader> pVSShader);

		//set cache param, m_VSShaderCustomValue 
		void SetVSShaderValue(const std::string& name, void* value);
		void DeleteVSShaderValue(const std::string& name);
		void SetVSShaderTexture(const std::string& name, Ref<Texture2D> pTex);
		void DeleteVSShaderTexture(const std::string& name);

		void SetPSShaderValue(const std::string& name, void* value);
		void DeletePSShaderValue(const std::string& name);
		void SetPSShaderTexture(const std::string& name, Ref<Texture2D> pTex);
		void DeletePSShaderTexture(const std::string& name);

		inline Ref<Material> GetMaterial() const
		{
			return m_pMaterial;
		}
	};

	struct MaterialShaderPara
	{
		Ref<Camera> pCamera;
		Ref<MaterialInstance> pMaterialInstance;
		//std::vector<Ref<LightComponent>> pLightArray;
		Ref<StaticMesh> m_pStaticMesh;
		//current render pass type
		uint32_t uiPassId;

		std::string m_ShaderPath;
	};
}
