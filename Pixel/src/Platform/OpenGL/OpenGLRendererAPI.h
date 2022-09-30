#pragma once

#include "Pixel/Renderer/RendererAPI.h"

namespace Pixel {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void ClearStencil() override;

		virtual void DepthMask(int32_t MaskBit) override;
		virtual void DepthTest(int32_t Bit) override;
		virtual void Blend(int32_t Bit) override;

		virtual void StencilTest(int32_t Bit) override;

		virtual void BindTexture(int32_t slot, uint32_t textureID) override;

		virtual void DrawIndexed(Primitive DrawMode, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void Cull(int32_t Bit) override;


		virtual void CullFrontOrBack(int32_t Bit) override;


		virtual void SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask) override;


		virtual void SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess) override;

		virtual void DepthFunc(DepthComp comp) override;

	};
}
