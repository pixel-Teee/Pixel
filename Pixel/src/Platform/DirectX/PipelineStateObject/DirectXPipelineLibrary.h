#pragma once

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"//d3d1x12

#include "Pixel/Renderer/PipelineStateObject/PipelineLibrary.h"

namespace Pixel {
	class DirectXPipelineLibrary : public PipelineLibrary
	{
	public:
		DirectXPipelineLibrary();

		virtual ~DirectXPipelineLibrary();

		virtual void Init() override;

		virtual void InsertNewName(std::wstring& name) override;

		virtual bool AlreadyExist(std::wstring& name) override;

		//TODO:need to modify to virtual
		void LoadGraphicsPipeline(std::wstring& psoName, Ref<PSO> pPso);

		virtual void StorePipeline(const std::wstring& psoName, Ref<PSO> pPso) override;
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineLibrary> m_PipelineLibrary;//pipeline state object library

		std::set<std::wstring> m_AlreadyExitsName;//already exists
	};
}