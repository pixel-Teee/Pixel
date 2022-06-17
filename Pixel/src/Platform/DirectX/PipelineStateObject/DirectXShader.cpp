#include "pxpch.h"

#include "DirectXShader.h"

#include <d3dcompiler.h>

#include <codecvt>

namespace Pixel {

	static std::wstring StringToWString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		return converter.from_bytes(str);
	}

	DirectXShader::DirectXShader(const std::string& filepath, const std::string& EntryPoint, const std::string& target)
	{
		//in terms of the filepath, to compile the shader
		m_pBlobShader = CompileShader(StringToWString(filepath), nullptr, EntryPoint, target);

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

		return byteCode;
	}

}

