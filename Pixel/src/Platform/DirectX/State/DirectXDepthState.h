#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/State/DepthState.h"

namespace Pixel {
	class DirectXDepthState : public DepthState
	{
	public:
		DirectXDepthState();
		virtual ~DirectXDepthState();

		D3D12_DEPTH_STENCIL_DESC m_DepthStencilDesc;

		virtual void SetDepthFunc(DepthFunc func) override;

		virtual void DepthTest(bool enable) override;

	};
}