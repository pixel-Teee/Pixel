#pragma once

#include "Pixel/ConstantBuffers.h"
#include "Pixel/Renderer/BaseRenderer.h"

namespace Pixel {
	class BufferLayout;
	class PSO;
	class Device;
	class RootSignature;
	class Shader;
	class Context;

	class EditorCamera;
	class Framebuffer;
	class DirectXRenderer : public BaseRenderer
	{
	public:
		DirectXRenderer();
		virtual ~DirectXRenderer();

		virtual uint32_t CreatePso(BufferLayout& layout) override;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer) override;

		virtual Ref<PSO> GetPso(uint32_t psoIndex) override;

	private:
		Ref<RootSignature> m_rootSignature;

		std::vector<Ref<PSO>> m_PsoArray;

		Ref<PSO> m_defaultPso;
		Ref<Shader> m_forwardPs;
		Ref<Shader> m_forwardVs;

		GlobalConstants m_globalConstants;
	};
}
 