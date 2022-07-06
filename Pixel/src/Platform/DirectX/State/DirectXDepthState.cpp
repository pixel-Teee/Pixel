#include "pxpch.h"

#include "DirectXDepthState.h"

namespace Pixel {

	DirectXDepthState::DirectXDepthState()
	{
		//m_DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_DepthStencilDesc.DepthEnable = true;
		m_DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		m_DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		m_DepthStencilDesc.StencilEnable = false;
		m_DepthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		m_DepthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		m_DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.BackFace = m_DepthStencilDesc.FrontFace;
	}

	DirectXDepthState::~DirectXDepthState()
	{

	}

	void DirectXDepthState::SetDepthFunc(DepthFunc func)
	{
		switch (func)
		{
		case DepthFunc::LESS:
			m_DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			break;
		case DepthFunc::LEQUAL:
			m_DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			break;
		}
		
	}

	void DirectXDepthState::DepthTest(bool enable)
	{
		m_DepthStencilDesc.DepthEnable = enable;
	}

}