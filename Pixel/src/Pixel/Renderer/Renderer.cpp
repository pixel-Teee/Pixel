#include "pxpch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Pixel/Renderer/3D/Renderer3D.h"
#include "Pixel/Renderer/3D/ShaderStringFactory.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Pixel {

	Renderer::SceneData* Renderer::m_SceneData = nullptr;

	void Renderer::Init()
	{
		RenderCommand::Init();
		//Renderer2D::Init();
		Renderer3D::Init();
		ShaderStringFactory::Init();

		m_SceneData = new Renderer::SceneData;
	}

	void Renderer::Finitialize()
	{
		RenderCommand::Finitialize();
		if (m_SceneData != nullptr) {
			delete m_SceneData;
			m_SceneData = nullptr;
		}
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		//其实ViewProjectionMatrix只要调用一次
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		//transform每个物体都不一样
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(Primitive::TRIANGLE, vertexArray);
	}
}