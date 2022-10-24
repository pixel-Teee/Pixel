#pragma once

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"//d3d1x12

#include "Pixel/Renderer/PipelineStateObject/PipelineLibrary.h"

namespace Pixel {
	class DirectXPipelineLibrary : public PipelineLibrary
	{
	public:
		DirectXPipelineLibrary();

		virtual void Init() override;
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineLibrary> m_PipelineLibrary;//pipeline state object library
	};
}