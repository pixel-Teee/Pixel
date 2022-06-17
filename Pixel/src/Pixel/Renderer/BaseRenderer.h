#pragma once

#include "Pixel/Scene/Components.h"

namespace Pixel {
	class BufferLayout;
	class Device;

	class EditorCamera;
	class Framebuffer;
	class Context;
	class PSO;
	//renderer interface
	class BaseRenderer
	{
	public:
		virtual ~BaseRenderer();

		virtual uint32_t CreatePso(BufferLayout& layout) = 0;

		virtual Ref<PSO> GetPso(uint32_t psoIndex) = 0;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer) = 0;

		static Ref<BaseRenderer> Create();
	};
}