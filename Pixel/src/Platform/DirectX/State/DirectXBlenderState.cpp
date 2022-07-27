#include "pxpch.h"

#include "DirectXBlenderState.h"

namespace Pixel {

	DirectXBlenderState::DirectXBlenderState()
	{
		//m_DefaultBlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		ZeroMemory(&m_DefaultBlendDesc, sizeof(D3D12_BLEND_DESC));
		//------Blend Describe------
		m_DefaultBlendDesc.AlphaToCoverageEnable = false;
		m_DefaultBlendDesc.IndependentBlendEnable = false;
		m_DefaultBlendDesc.RenderTarget[0].BlendEnable = false;
		m_DefaultBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		m_DefaultBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		m_DefaultBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		m_DefaultBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		//------Blend Describe------

		m_DefaultBlendDesc.RenderTarget[2].BlendEnable = false;
		m_DefaultBlendDesc.RenderTarget[2].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[2].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[2].BlendOp = D3D12_BLEND_OP_ADD;
		m_DefaultBlendDesc.RenderTarget[2].SrcBlendAlpha = D3D12_BLEND_ONE;
		m_DefaultBlendDesc.RenderTarget[2].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		m_DefaultBlendDesc.RenderTarget[2].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		m_DefaultBlendDesc.RenderTarget[2].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		m_BlendDisable = m_DefaultBlendDesc;

		m_CurrentBlend = m_BlendDisable;
	}

	DirectXBlenderState::~DirectXBlenderState()
	{

	}

	void DirectXBlenderState::SetBlendState(BlenderStateType type)
	{
		switch (type)
		{
		case BlenderStateType::BlendDisable:
		{
			m_CurrentBlend = m_BlendDisable;
			break;
		}
		}
	}

	void DirectXBlenderState::SetRenderTargetBlendState(uint32_t index, bool enableBlend)
	{
		m_CurrentBlend.RenderTarget[index].BlendEnable = enableBlend;
	}

	void DirectXBlenderState::SetIndependentBlendEnable(bool enable)
	{
		m_CurrentBlend.IndependentBlendEnable = enable;
	}

}