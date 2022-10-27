#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	enum class ShaderType
	{
		VertexShader,
		PixelShader
	};

	class Context;
	class DescriptorHandle;
	class ShaderKey;
	class Material;
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

		virtual void SetData(const std::string& name, void* data) = 0;

		virtual void SubmitData(Ref<Context> pContext) = 0;

		virtual void SubmitData(Ref<Context> pContext, const std::string& cbvName) = 0;

		virtual void ResetTextureDescriptor() = 0;

		virtual void SetTextureDescriptor(const std::string& name, Ref<DescriptorHandle> pDescriptorHandle) = 0;

		virtual void SubmitTextureDescriptor(Ref<Context> pContext, Ref<DescriptorHandle> pDescriptorHeapHandle) = 0;

		static Ref<Shader> Create(const std::string& filepath);
		//static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filepath, const std::string& EntryPoint, const std::string& target, bool isGenerated = false, Ref<Material> pMaterial = nullptr);

		static Ref<Shader> Create(const std::string& filePath, const std::string& EntryPoint, const std::string& target, Ref<ShaderKey> pShaderKey, Ref<Material> pMaterial = nullptr);

		//static Ref<Shader> Create(const std::string& EntryPoint, const std::string& target, Ref<ShaderKey> pShaderKey, const std::string& shaderCode);
	};
}
