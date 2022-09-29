#include "pxpch.h"

#include "DirectXShader.h"

#include <d3dcompiler.h>

#include <codecvt>

#include "Pixel/Math/Math.h"
#include "DirectXCbvShaderParameter.h"
#include "DirectXSrvShaderParameter.h"
#include "DirectXRootSignature.h"
#include "Pixel/Renderer/Sampler/Sampler.h"

namespace Pixel {

	static std::wstring StringToWString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		return converter.from_bytes(str);
	}

	DirectXShader::DirectXShader(const std::string& filepath, const std::string& EntryPoint, const std::string& target, bool IsGenerated)
	{
		//in terms of the filepath, to compile the shader
		m_pBlobShader = CompileShader(StringToWString(filepath), nullptr, EntryPoint, target);

		m_IsGenerated = IsGenerated;
		
		if (target == "vs_5_0")
		{
			m_ShaderType = ShaderType::VertexShader;
		}
		else if (target == "ps_5_0")
		{
			m_ShaderType = ShaderType::PixelShader;
		}

		if (!IsGenerated)
		{
			//get the reflection information to create shader parameter
			Microsoft::WRL::ComPtr<ID3D12ShaderReflection> pReflection;

			D3DReflect(m_pBlobShader->GetBufferPointer(), m_pBlobShader->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)pReflection.GetAddressOf());

			D3D12_SHADER_DESC shaderDesc;
			pReflection->GetDesc(&shaderDesc);

			m_MaxBindPointIndex = 0;

			for (int32_t i = 0; i < shaderDesc.BoundResources; ++i)
			{
				D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
				pReflection->GetResourceBindingDesc(i, &resourceDesc);

				auto bindResourceName = resourceDesc.Name;
				auto registerSpace = resourceDesc.Space;//temporarily don't use, just use space 0
				auto resourceType = resourceDesc.Type;
				auto bindPoint = resourceDesc.BindPoint;
				auto bindCount = resourceDesc.BindCount;

				if (resourceType == D3D_SIT_CBUFFER)
				{
					//cbv
					ID3D12ShaderReflectionConstantBuffer* constBuffer = pReflection->GetConstantBufferByName(bindResourceName);
					D3D12_SHADER_BUFFER_DESC bufferDesc;
					constBuffer->GetDesc(&bufferDesc);

					Ref<DirectXCbvShaderParameter> pCbvShaderParameter = CreateRef<DirectXCbvShaderParameter>();
					pCbvShaderParameter->m_Name = bindResourceName;
					pCbvShaderParameter->m_Size = bufferDesc.Size;
					pCbvShaderParameter->m_BindPoint = bindPoint;

					for (int32_t j = 0; j < bufferDesc.Variables; ++j)
					{
						ID3D12ShaderReflectionVariable* variableInterface = constBuffer->GetVariableByIndex(j);

						D3D12_SHADER_VARIABLE_DESC variableDesc;
						variableInterface->GetDesc(&variableDesc);

						//std::cout << "variable name " << variableDesc.Name << std::endl;
						//std::cout << "variable offset " << variableDesc.StartOffset << std::endl;
						//std::cout << "variable size " << variableDesc.Size << std::endl;

						CbvVariableParameter cbvVariableParameter;
						cbvVariableParameter.m_VariableName = variableDesc.Name;
						cbvVariableParameter.m_StartOffset = variableDesc.StartOffset;
						cbvVariableParameter.m_Size = variableDesc.Size;
						pCbvShaderParameter->m_CbvVariableParameters.push_back(cbvVariableParameter);//push back a variable
					}
					m_CbvShaderParameter.push_back(pCbvShaderParameter);
					m_AlignedCbvSize.push_back(Math::AlignUpWithMask(bufferDesc.Size, 255));
				}
				else if (resourceType == D3D_SIT_TEXTURE)
				{
					//srv
					Ref<DirectXSrvShaderParameter> pSrvShaderParameter = CreateRef<DirectXSrvShaderParameter>();
					pSrvShaderParameter->m_Name = bindResourceName;
					pSrvShaderParameter->m_BindPoint = bindPoint;
					m_MaxBindPointIndex = std::max(m_MaxBindPointIndex, bindPoint);
					m_SrvShaderParameter.push_back(pSrvShaderParameter);
				}
			}

			//std::cout << "test" << std::endl;
		}
	}

	DirectXShader::~DirectXShader()
	{

	}

	void DirectXShader::Bind() const
	{

	}

	void DirectXShader::Unbind() const
	{

	}

	void DirectXShader::SetInt(const std::string& name, int value)
	{

	}

	void DirectXShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{

	}

	void DirectXShader::SetFloat(const std::string& name, float value)
	{

	}

	void DirectXShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{

	}

	void DirectXShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{

	}

	void DirectXShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{

	}

	void DirectXShader::SetMat4(const std::string& name, const glm::mat4& value)
	{

	}

	const std::string& DirectXShader::GetName() const
	{
		return "";
	}


	std::pair<void*, uint64_t> DirectXShader::GetShaderBinary()
	{
		return std::make_pair(m_pBlobShader->GetBufferPointer(), m_pBlobShader->GetBufferSize());
	}

	ShaderType DirectXShader::GetShaderType()
	{
		return m_ShaderType;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> DirectXShader::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entryPoint, const std::string& target)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		PX_CORE_ASSERT(hr >= 0, "compile shaders error!");

		//------get reflection information------
		//Microsoft::WRL::ComPtr<ID3D12ShaderReflection> pReflection;
		//
		//D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)pReflection.GetAddressOf());
		//
		//D3D12_SHADER_DESC shaderDesc;
		//pReflection->GetDesc(&shaderDesc);
		//
		//std::wcout << filename << std::endl;
		//for (int32_t i = 0; i < shaderDesc.BoundResources; ++i)
		//{
		//	D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
		//	pReflection->GetResourceBindingDesc(i, &resourceDesc);
		//
		//	auto shaderVariableName = resourceDesc.Name;
		//	auto registerSpace = resourceDesc.Space;
		//	auto resourceType = resourceDesc.Type;
		//	auto bindPoint = resourceDesc.BindPoint;
		//	auto bindCount = resourceDesc.BindCount;
		//
		//	std::cout << "variable name " << shaderVariableName << std::endl;
		//	std::cout << "type name is " << resourceType << std::endl;
		//	std::cout << "bind point is " << bindPoint << std::endl;
		//	std::cout << "register space is " << registerSpace << std::endl;
		//	std::cout << "bind count " << bindCount << std::endl;
		//}
		//------get reflection information------

		//Microsoft::WRL::ComPtr<ID3D12ShaderReflection> pReflection;
		//
		//D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)pReflection.GetAddressOf());
		//
		//D3D12_SHADER_DESC shaderDesc;
		//pReflection->GetDesc(&shaderDesc);

		//std::wcout << filename << std::endl;

		//for (int32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
		//{
		//	ID3D12ShaderReflectionConstantBuffer* constBuffer = pReflection->GetConstantBufferByIndex(i);
		//	
		//	D3D12_SHADER_BUFFER_DESC bufferDesc;
		//	constBuffer->GetDesc(&bufferDesc);
		//	
		//	std::cout << "const buffer name" << " " << bufferDesc.Name << std::endl;
		//	std::cout << "const buffer size" << " " << bufferDesc.Size << std::endl;
		//	for (int32_t j = 0; j < bufferDesc.Variables; ++j)
		//	{
		//		ID3D12ShaderReflectionVariable* variableInterface = constBuffer->GetVariableByIndex(j);
		//
		//		D3D12_SHADER_VARIABLE_DESC variableDesc;
		//		variableInterface->GetDesc(&variableDesc);
		//
		//		std::cout << "variable name " << variableDesc.Name << std::endl;
		//		std::cout << "variable offset " << variableDesc.StartOffset << std::endl;
		//		std::cout << "variable size " << variableDesc.Size << std::endl;
		//	}
		//	std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		//}
		//std::cout << "*****************************************" << std::endl;

		//for (int32_t i = 0; i < shaderDesc.BoundResources; ++i)
		//{
		//	D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
		//	pReflection->GetResourceBindingDesc(i, &resourceDesc);
		//
		//	auto shaderVariableName = resourceDesc.Name;
		//	auto registerSpace = resourceDesc.Space;
		//	auto resourceType = resourceDesc.Type;
		//	auto bindPoint = resourceDesc.BindPoint;
		//	auto bindCount = resourceDesc.BindCount;
		//
		//	//std::cout << "variable name " << shaderVariableName << std::endl;
		//	//std::cout << "type name is " << resourceType << std::endl;
		//	//std::cout << "bind point is " << bindPoint << std::endl;
		//	//std::cout << "register space is " << registerSpace << std::endl;
		//	//std::cout << "bind count " << bindCount << std::endl;
		//
		//	//texture
		//	if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
		//	{
		//		std::cout << "variable name" << shaderVariableName << std::endl;
		//		std::cout << "type name is " << resourceType << std::endl;
		//		std::cout << "bind point is " << bindPoint << std::endl;
		//		std::cout << "register space is " << registerSpace << std::endl;
		//		std::cout << "bind count " << bindCount << std::endl;
		//	}
		//
		//	std::cout << "**************************************************" << std::endl;
		//}

		return byteCode;
	}

}

