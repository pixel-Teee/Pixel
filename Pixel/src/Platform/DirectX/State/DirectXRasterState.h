#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/State/RasterState.h"

namespace Pixel {
	class DirectXRasterState : public RasterState
	{
	public:
		DirectXRasterState();
		virtual ~DirectXRasterState();

		D3D12_RASTERIZER_DESC m_RasterizerDefault;
	};
}