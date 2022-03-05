#include "pxpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Pixel
{
	/*--------------------------------------
	-------------Stencil--------------------
	----------------------------------------*/
	static GLenum StencilFuncToOpenGLStencilFunc(StencilFunc func)
	{
		switch (func)
		{
		case ALWAYS:
			return GL_ALWAYS;
		case NOTEQUAL:
			return GL_NOTEQUAL;
		}
		PX_CORE_ASSERT(false, "Unknow StencilFunc");
		return -1;
	}

	static GLenum StencilOpToOpenGLStencilOp(StencilOp op)
	{
		switch (op)
		{
		case KEEP:
			return GL_KEEP;
		case INCREMENT:
			return GL_INCR_WRAP;
		case DECREMENT:
			return GL_DECR_WRAP;
		}
		PX_CORE_ASSERT(false, "Unknown StencilOp");
		return -1;
	}

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DepthMask(int32_t MaskBit)
	{	
		if(MaskBit) glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::DepthTest(int32_t Bit)
	{
		if(Bit) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::Blend(int32_t Bit)
	{
		if (Bit)
		{
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void OpenGLRendererAPI::StencilTest(int32_t Bit)
	{
		if(Bit) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::Cull(int32_t Bit)
	{
		if(Bit) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::CullFrontOrBack(int32_t Bit)
	{
		if(Bit) glCullFace(GL_FRONT);
		else glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask)
	{
		glStencilFunc(StencilFuncToOpenGLStencilFunc(stencilFunc), ref, mask);
	}

	void OpenGLRendererAPI::SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess)
	{
		//Front
		if (FrontOrBack)
		{
			glStencilOpSeparate(GL_FRONT, StencilOpToOpenGLStencilOp(stencilFail), StencilOpToOpenGLStencilOp(depthFail), StencilOpToOpenGLStencilOp(depthSuccess));
		}
		else
		{
			glStencilOpSeparate(GL_BACK, StencilOpToOpenGLStencilOp(stencilFail), StencilOpToOpenGLStencilOp(depthFail), StencilOpToOpenGLStencilOp(depthSuccess));
		}
	}

	void OpenGLRendererAPI::BindTexture(int32_t slot, uint32_t textureID)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

}