#include "pxpch.h"

#include "DirectXRasterState.h"

namespace Pixel {

	DirectXRasterState::DirectXRasterState()
	{
		//m_RasterizerDefault = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_RasterizerDefault.FillMode = D3D12_FILL_MODE_SOLID;
		m_RasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
		m_RasterizerDefault.FrontCounterClockwise = false;
		m_RasterizerDefault.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		m_RasterizerDefault.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		m_RasterizerDefault.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		m_RasterizerDefault.DepthClipEnable = true;
		m_RasterizerDefault.MultisampleEnable = false;
		m_RasterizerDefault.AntialiasedLineEnable = false;
		m_RasterizerDefault.ForcedSampleCount = 0;
		m_RasterizerDefault.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	void DirectXRasterState::SetCullMode(CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::None:
			m_RasterizerDefault.CullMode = D3D12_CULL_MODE_NONE;
			break;
		case CullMode::Back:
			m_RasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
			break;
		case CullMode::Front:
			m_RasterizerDefault.CullMode = D3D12_CULL_MODE_FRONT;
			break;
		}
	}

	DirectXRasterState::~DirectXRasterState()
	{

	}

}