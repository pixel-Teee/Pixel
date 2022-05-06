#pragma once

#include "RendererAPI.h"
namespace Pixel {

	class RenderCommand
	{
	public:
		static void Init();
		inline static void Finitialize()
		{
			if (s_RendererAPI != nullptr)
			{
				delete s_RendererAPI;
				s_RendererAPI = nullptr;
			}
		}
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		//color
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

		//Open/Close Depth Write
		inline static void DepthMask(int32_t MaskBit)
		{
			s_RendererAPI->DepthMask(MaskBit);
		}

		//Open/Close Depth Test
		inline static void DepthTest(int32_t Bit)
		{
			s_RendererAPI->DepthTest(Bit);
		}

		inline static void DepthFunc(DepthComp comp)
		{
			s_RendererAPI->DepthFunc(comp);
		}

		//blend
		inline static void Blend(int32_t Bit)
		{
			s_RendererAPI->Blend(Bit);
		}

		inline static void BindTexture(int32_t slot, uint32_t textureID)
		{
			s_RendererAPI->BindTexture(slot, textureID);
		}

		//stencil
		inline static void StencilTest(int32_t Bit)
		{
			s_RendererAPI->StencilTest(Bit);
		}

		inline static void SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask)
		{
			s_RendererAPI->SetStencilFunc(stencilFunc, ref, mask);
		}

		inline static void SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess)
		{
			s_RendererAPI->SetFrontOrBackStencilOp(FrontOrBack, stencilFail, depthFail, depthSuccess);
		}

		inline static void ClearStencil()
		{
			s_RendererAPI->ClearStencil();
		}

		//cull
		inline static void Cull(int32_t Bit)
		{
			s_RendererAPI->Cull(Bit);
		}

		inline static void CullFrontOrBack(int32_t Bit)
		{
			s_RendererAPI->CullFrontOrBack(Bit);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}