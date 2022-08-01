#include "pxpch.h"

//------my libary------
#include "PhysicsDraw.h"
#include "Pixel/Renderer/3D/Renderer3D.h"
//------my libary------

namespace Pixel {

	void PhysicsDraw::SetMatrix(glm::mat4 ViewProjection)
	{
		m_ViewProjection = ViewProjection;
	}

	void PhysicsDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
		
		glm::vec3 Point1 = { from.x(), from.y(), from.z() };
		glm::vec3 Point2 = { to.x(), to.y(), to.z() };
		glm::vec3 Color = { fromColor.x(), fromColor.y(), fromColor.z() };
		Renderer3D::DrawLine(Point1, Point2, Color, m_ViewProjection);
	}

	void PhysicsDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		glm::vec3 Point1 = { from.x(), from.y(), from.z() };
		glm::vec3 Point2 = { to.x(), to.y(), to.z() };
		glm::vec3 Color = { color.x(), color.y(), color.z() };

		//PIXEL_CORE_TRACE("{0}, {1}, {2}", Point1.x, Point1.y, Point1.z);

		Renderer3D::DrawLine(Point1, Point2, Color, m_ViewProjection);
	}

	void PhysicsDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void PhysicsDraw::reportErrorWarning(const char* warningString)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void PhysicsDraw::draw3dText(const btVector3& location, const char* textString)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void PhysicsDraw::setDebugMode(int debugMode)
	{
		m_DebugDrawMode = debugMode;
	}

	int PhysicsDraw::getDebugMode() const
	{
		return m_DebugDrawMode;
	}

}