#pragma once

#include <cstring>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Pixel/Renderer/3D/ShaderKey.h"

namespace Pixel {
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		//static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filepath, const std::string& EntryPoint, const std::string& target);

		Ref<ShaderKey> m_pShaderKey;
	};

	class ShaderLibrary
	{
	public:
	//ShaderKey is a bunch of marco
	using ShaderSet = std::map<Ref<ShaderKey>, Ref<Shader>>;
	public:
		ShaderLibrary() = default;
		ShaderLibrary(const std::string& Name);
		~ShaderLibrary();
		//(shadername, shaderset)
		void SetShader(const std::string& Name, Ref<ShaderKey> Key, Ref<Shader> pShader);

		Ref<ShaderSet> GetShaderSet(const std::string& Name);

		void DeleteShaderSet(const std::string& Name);

		//in terms of name and key, get the shader
		Ref<Shader> GetShader(const std::string& Name, Ref<ShaderKey> Key);

		void DeleteShader(const std::string& Name, Ref<ShaderKey> Key);

	private:
		//name is shader name
		std::map<std::string, Ref<ShaderSet>> m_ShaderMap;

		//name is pass or shader type
		std::string m_MapName;
	};

	//cache
	class ShaderResourceManager
	{
	public:
		static Ref<ShaderLibrary> GetMaterialShaderMap()
		{
			static Ref<ShaderLibrary> pMaterialMap = CreateRef<ShaderLibrary>("MaterialShaderMap");

			return pMaterialMap;
		}

		//TODO:implement this
		static bool CacheShader();

		static Ref<Shader> CreateShader(MaterialShaderPara& MSPara, uint32_t uiPassType, uint32_t uiShaderId);
	};
}
