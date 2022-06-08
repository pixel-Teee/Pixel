#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"

namespace Pixel {

	class DirectXRootParameter : public RootParameter
	{
	public:
		DirectXRootParameter();
		virtual ~DirectXRootParameter();
		virtual void InitAsDescriptorRange(RangeType Type, uint32_t Register,
			uint32_t Count, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) override;
		virtual void InitAsDescriptorTable(uint32_t RangeCount, ShaderVisibility Visibility = ShaderVisibility::ALL) override;
		virtual void SetTableRange(uint32_t RangeIndex, RangeType Type, uint32_t Register, uint32_t Count, uint32_t Space = 0) override;


		virtual void InitAsConstants(uint32_t Register, uint32_t NumDwords, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) override;
		virtual void InitAsConstantBuffer(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) override;


		virtual void InitiAsBufferSRV(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) override;
		virtual void InitAsBufferUAV(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) override;

		virtual void Clear() override;

		D3D12_ROOT_PARAMETER GetNativeRootParameter() { return m_RootParam; }
	private:	
		D3D12_ROOT_PARAMETER m_RootParam;
	};
}