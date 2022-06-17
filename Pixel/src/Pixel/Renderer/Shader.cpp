#include "pxpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Pixel/Renderer/3D/ShaderStringFactory.h"
#include "Pixel/Renderer/3D/Material.h"
#include "Platform/DirectX/PipelineStateObject/DirectXShader.h"

namespace Pixel {

	//Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	//		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
	//	}

	//	PX_CORE_ASSERT(false, "Unknown RendererAPI!");
	//	return nullptr;
	//}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filepath, const std::string& EntryPoint, const std::string& target)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXShader>(filepath, EntryPoint, target);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	////////////////////
	///Shader Library///
	////////////////////

	ShaderLibrary::ShaderLibrary(const std::string& Name)
	{
		m_MapName = Name;
	}

	ShaderLibrary::~ShaderLibrary()
	{

	}
	
	void ShaderLibrary::SetShader(const std::string& Name, Ref<ShaderKey> Key, Ref<Shader> pShader)
	{
		Ref<ShaderSet> pSet = CreateRef<ShaderSet>();
		pSet->insert(std::make_pair(Key, pShader));
		m_ShaderMap[Name] = pSet;
	}

	Ref<ShaderLibrary::ShaderSet> ShaderLibrary::GetShaderSet(const std::string& Name)
	{
		return m_ShaderMap[Name];
	}

	void ShaderLibrary::DeleteShaderSet(const std::string& Name)
	{
		m_ShaderMap.erase(Name);
	}

	Ref<Shader> ShaderLibrary::GetShader(const std::string& Name, Ref<ShaderKey> Key)
	{
		Ref<ShaderSet> pSet = m_ShaderMap[Name];
		auto it = pSet->find(Key);
		return it->second;
	}

	void ShaderLibrary::DeleteShader(const std::string& Name, Ref<ShaderKey> Key)
	{
		Ref<ShaderSet> pSet = m_ShaderMap[Name];
		pSet->erase(Key);
	}

	bool ShaderResourceManager::CacheShader()
	{
		return true;
	}

	Ref<Shader> ShaderResourceManager::CreateShader(MaterialShaderPara& MSPara, uint32_t uiPassType, uint32_t uiShaderId)
	{
		if (!MSPara.m_pStaticMesh || !MSPara.pMaterialInstance)
			return nullptr;

		Ref<Shader> pShader;

		Ref<Material> pMaterial = MSPara.pMaterialInstance->GetMaterial();
		
		std::string filePath;

		//get the shader filepath
		//generater the shader text
		if (!ShaderStringFactory::CreateShaderString(pShader, MSPara, uiPassType, filePath))
		{
			return nullptr;
		}

		//from the disk to load shader and compile the shader
		pShader = Shader::Create(filePath);
		pShader->m_pShaderKey = CreateRef<ShaderKey>();
		ShaderKey::SetMaterialShaderKey(pShader->m_pShaderKey, MSPara, uiPassType);

		return pShader;
	}

}