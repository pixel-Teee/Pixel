#pragma once

#include "Pixel/Renderer/Shader.h"

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"

namespace Pixel {
	class DirectXCbvShaderParameter;
	class DirectXSrvShaderParameter;
	class DirectXRootSignature;

	enum class ShaderType
	{
		VertexShader,
		PixelShader
	};

	class DirectXShader : public Shader
	{
	public:
		DirectXShader(const std::string& filepath, const std::string& EntryPoint, const std::string& target, bool IsGenerated = false);
		virtual ~DirectXShader();
		//------garbage------
		virtual void Bind() const override;

		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;

		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

		virtual void SetFloat(const std::string& name, float value) override;

		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;

		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;

		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override;
		//------garbage------

		std::pair<void*, uint64_t> GetShaderBinary();

		ShaderType GetShaderType();
	private:

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename,
		const D3D_SHADER_MACRO* defines, const std::string& entryPoint, const std::string& target);

		//TODO:in the future, will be to desgin a cache scheme
		Microsoft::WRL::ComPtr<ID3DBlob> m_pBlobShader;

		//will add sampler state?
		std::vector<Ref<DirectXCbvShaderParameter>> m_CbvShaderParameter;
		std::vector<Ref<DirectXSrvShaderParameter>> m_SrvShaderParameter;
		uint32_t m_MaxBindPointIndex;//max bind point

		std::vector<uint32_t> m_AlignedCbvSize;

		bool m_IsGenerated;

		Ref<DirectXRootSignature> m_pRootSignature;

		ShaderType m_ShaderType;
		//uint32_t m_AlignedCbvSize;

		friend class DirectXRenderer;
	};
}
