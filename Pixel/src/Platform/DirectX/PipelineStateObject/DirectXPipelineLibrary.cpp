#include "pxpch.h"

#include "DirectXPipelineLibrary.h"

#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"

namespace Pixel {
	DirectXPipelineLibrary::DirectXPipelineLibrary()
	{
	
	}

	DirectXPipelineLibrary::~DirectXPipelineLibrary()
	{

	}

	void DirectXPipelineLibrary::Init()
	{
		//initialize pipeline library

		//TODO:in the future, first from file to create pipeline library, if create error, destory file, then create pipeline library again

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(m_PipelineLibrary.GetAddressOf())) >= 0,
		"create pipeline library error!");

		/*
		switch (hr)
		{
		case DXGI_ERROR_UNSUPPORTED://driver doesn't support pipeline libraries, WDDM2.1 drivers must support it
			break;
		case E_INVALIDARG://the provided library is corrupted or unrecognized
		case D3D12_ERROR_ADAPTER_NOT_FOUND://the provided library contains data for different hardware
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH://the provided library contains datd from an old driver or runtime, we need to re-create it

		}
		*/
	}

	void DirectXPipelineLibrary::InsertNewName(std::wstring& name)
	{
		m_AlreadyExitsName.insert(name);//insert name
	}

	bool DirectXPipelineLibrary::AlreadyExist(std::wstring& name)
	{
		return m_AlreadyExitsName.find(name) != m_AlreadyExitsName.end();//find already name
	}

	void DirectXPipelineLibrary::LoadGraphicsPipeline(std::wstring& psoName, Ref<PSO> pPso)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject;
		//load the pipeline state object
		m_PipelineLibrary->LoadGraphicsPipeline(psoName.c_str(), &(std::static_pointer_cast<GraphicsPSO>(pPso)->m_PSODesc), IID_PPV_ARGS(pipelineStateObject.GetAddressOf()));

		//set to pPso
		std::static_pointer_cast<GraphicsPSO>(pPso)->m_pPSO = pipelineStateObject;
	}

	void DirectXPipelineLibrary::StorePipeline(const std::wstring& psoName, Ref<PSO> pPso)
	{
		//TODO:temporarily store the graphics pso

		Ref<GraphicsPSO> pGraphicsPso = std::static_pointer_cast<GraphicsPSO>(pPso);

		//store the pso
		HRESULT hr = m_PipelineLibrary->StorePipeline(psoName.c_str(), pGraphicsPso->GetPipelineStateObject().Get());

		m_AlreadyExitsName.insert(psoName);

		switch (hr)
		{
		case E_INVALIDARG:
			PX_CORE_ASSERT(false, "name already exists!");
		case E_OUTOFMEMORY:
			PX_CORE_ASSERT(false, "unable to allocate storage in the library!");
		}
	}

}