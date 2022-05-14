#pragma once

#include "LinearMath/btIDebugDraw.h"

#include "glm/glm.hpp"

namespace Pixel {
	class PhysicsDraw : public btIDebugDraw
	{
	public:
		void SetMatrix(glm::mat4 viewProjection);
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
		virtual void setDebugMode(int debugMode);
		virtual int getDebugMode() const override;
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
		virtual void reportErrorWarning(const char* warningString) override;
		virtual void draw3dText(const btVector3& location, const char* textString) override;
	private:
		int m_DebugDrawMode;
		glm::mat4 m_ViewProjection;
	};
}
