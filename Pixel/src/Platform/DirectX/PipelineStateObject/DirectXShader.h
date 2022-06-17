#pragma once

#include "Pixel/Renderer/Shader.h"

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"

namespace Pixel {
	class DirectXShader : public Shader
	{
	public:
		DirectXShader(const std::string& filepath, const std::string& EntryPoint, const std::string& target);
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
	private:
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename,
		const D3D_SHADER_MACRO* defines, const std::string& entryPoint, const std::string& target);

		Microsoft::WRL::ComPtr<ID3DBlob> m_pBlobShader;
	};
}
