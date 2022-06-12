#pragma once

#include "Platform/DirectX/d3dx12.h"

#include "Pixel/Renderer/RendererType.h"
#include "Pixel/Renderer/State/BlenderState.h"

namespace Pixel {

	class DirectXBlenderState : public BlenderState
	{
	public:
		DirectXBlenderState();
		~DirectXBlenderState();

		virtual void SetBlendState(BlenderStateType type) override;

		//independent blend enable?
		//void SetIndependentBlendEnable(bool enable);
	
		D3D12_BLEND_DESC m_DefaultBlendDesc;

		D3D12_BLEND_DESC m_BlendDisable;

		D3D12_BLEND_DESC m_CurrentBlend;
	};
}