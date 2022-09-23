#pragma once

#include "entt.hpp"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"

namespace Pixel {
	//just for graph node editor's preview
	class SimpleScene
	{
	public:
		SimpleScene();
		
		void OnUpdateEditorDeferred(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pGeoFrameBuffer, Ref<Framebuffer>& pLightFrameBuffer, Ref<Framebuffer>& pFinalFrameBuffer);

	private:
		entt::registry m_Registry;
	};
}