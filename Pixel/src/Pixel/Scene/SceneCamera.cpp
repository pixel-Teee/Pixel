#include "pxpch.h"

#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Pixel {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{	
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}	

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}	
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::SceneCamera>("SceneCamera")
		.constructor<>()
		.property("m_ProjectionType", &Pixel::SceneCamera::m_ProjectionType)
		.property("m_OrthographicSize", &Pixel::SceneCamera::m_OrthographicSize)
		.property("m_OrthographicNear", &Pixel::SceneCamera::m_OrthographicNear)
		.property("m_OrthographicFar", &Pixel::SceneCamera::m_OrthographicFar)
		.property("m_PerspectiveFOV", &Pixel::SceneCamera::m_PerspectiveFOV)
		.property("m_PerspectiveNear", &Pixel::SceneCamera::m_PerspectiveNear)
		.property("m_PerspectiveFar", &Pixel::SceneCamera::m_PerspectiveFar)
		.property("m_AspectRatio", &Pixel::SceneCamera::m_AspectRatio);
}