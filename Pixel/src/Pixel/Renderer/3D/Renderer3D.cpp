#include "pxpch.h"

#include "Renderer3D.h"

#include "BaseType.h"

#include "Model.h"

namespace Pixel {
	static Ref<Shader> m_Shader;

	void Renderer3D::Init()
	{	
		m_Shader = Shader::Create("assets/shaders/Common.glsl");
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, int EntityID)
	{
		MeshComponent.mesh.Draw(transform, m_Shader, EntityID);
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		glm::mat4 viewProj = camera.GetViewProjection();

		m_Shader->Bind();
		m_Shader->SetMat4("u_ViewProjection", viewProj);
	}

	void Renderer3D::EndScene()
	{
		m_Shader->Unbind();
	}
}