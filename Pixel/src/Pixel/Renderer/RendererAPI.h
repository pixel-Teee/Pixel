#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Pixel {
	//stencil func and op
	enum StencilFunc
	{
		ALWAYS,
		NOTEQUAL
	};

	enum StencilOp
	{
		KEEP,
		INCREMENT,
		DECREMENT
	};

	enum DepthComp
	{
		EQUAL,
		LEQUAL,
		LESS
	};

	enum Primitive
	{
		LINE,
		TRIANGLE
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, DirectX12 = 2
		};
	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		//clear color buffer 
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Primitive DrawMode, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		//depth
		virtual void DepthTest(int32_t Bit) = 0;
		virtual void DepthMask(int32_t MaskBit) = 0;
		virtual void DepthFunc(DepthComp comp) = 0;

		//blend
		virtual void Blend(int32_t Bit) = 0;

		virtual void BindTexture(int32_t slot, uint32_t textureID) = 0;

		//stencil
		virtual void SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask) = 0;
		virtual void SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess) = 0;
		virtual void StencilTest(int32_t Bit) = 0;
		virtual void ClearStencil() = 0;

		//cull
		virtual void Cull(int32_t Bit) = 0;
		virtual void CullFrontOrBack(int32_t Bit) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}