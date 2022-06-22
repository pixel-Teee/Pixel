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
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) = 0;

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer) = 0;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) = 0;

		static Ref<BaseRenderer> Create();
	};
}