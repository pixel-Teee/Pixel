#include "pxpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Pixel {

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

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

	////////////////////
	///Shader Library///
	////////////////////

	ShaderLibrary ShaderLibrary::shaderlibrary;

	void ShaderLibrary::Init()
	{
		shaderlibrary.m_Shaders.clear();
	}

	ShaderLibrary ShaderLibrary::GetShaderLibrary()
	{
		return shaderlibrary;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		PX_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{	
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	void ShaderLibrary::Test(const std::string& filepath)
	{
		//auto& TestShader = std::make_shared<SpirvShader>("assets/shaders/Test.spriv");	
	}


	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}