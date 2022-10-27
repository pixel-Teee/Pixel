#pragma once

#include "Pixel/Renderer/Shader.h"

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"

namespace Pixel {
	class DirectXCbvShaderParameter;
	class DirectXSrvShaderParameter;
	class DirectXRootSignature;
	class DescriptorHandle;
	class DescriptorHeap;
	class Material;

	class DirectXShader : public Shader
	{
	public:
		DirectXShader(const std::string& filepath, const std::string& EntryPoint, const std::string& target, bool IsGenerated = false, Ref<Material> pMaterial = nullptr);

		DirectXShader(const std::string& filePath, const std::string& EntryPoint, const std::string& target, Ref<ShaderKey> pShaderKey, Ref<Material> pMaterial = nullptr);

		//DirectXShader(const std::string& EntryPoint, const std::string& target, Ref<ShaderKey> pShaderKey, const std::string& shaderCode, Ref<Material> pMaterial);
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

		virtual void SetData(const std::string& name, void* data) override;

		virtual void SubmitData(Ref<Context> pContext) override;
 
		virtual void SubmitData(Ref<Context> pContext, const std::string& cbvName) override;

		virtual void ResetTextureDescriptor() override;

		virtual void SetTextureDescriptor(const std::string& name, Ref<DescriptorHandle> pDescriptorHandle) override;

		virtual void SubmitTextureDescriptor(Ref<Context> pContext, Ref<DescriptorHandle> pDescriptorHeapHandle) override;

		std::pair<void*, uint64_t> GetShaderBinary();

		ShaderType GetShaderType();
	private:

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const std::string& entryPoint, const std::string& target, Ref<ShaderKey> pShaderKey = nullptr);

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& shaderCode, const std::string& entryPoint, const std::string& target, Ref<ShaderKey> pShaderKey = nullptr);

		//TODO:in the future, will be to desgin a cache scheme
		Microsoft::WRL::ComPtr<ID3DBlob> m_pBlobShader;

		//will add sampler state?
		std::vector<Ref<DirectXCbvShaderParameter>> m_CbvShaderParameter;
		std::vector<Ref<DirectXSrvShaderParameter>> m_SrvShaderParameter;
		uint32_t m_MaxBindPointIndex;//max bind point

		std::vector<uint32_t> m_AlignedCbvSize;

		bool m_IsGenerated;

		//Ref<DirectXRootSignature> m_pRootSignature;

		ShaderType m_ShaderType;
		//uint32_t m_AlignedCbvSize;

		char** m_DataCache;

		std::vector<Ref<DescriptorHandle>> m_SrvDescriptorHandleCache;//reference texture descriptor handle

		friend class DirectXRenderer;
	};
}
