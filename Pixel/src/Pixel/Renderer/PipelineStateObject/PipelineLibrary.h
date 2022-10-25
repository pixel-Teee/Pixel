#pragma once

namespace Pixel {
	class PSO;
	//pipeline state object library
	class PipelineLibrary
	{
	public:
		virtual void Init() = 0;//initialize pipeline library

		virtual void InsertNewName(std::wstring& name) = 0;//insert new name

		virtual void StorePipeline(const std::wstring& psoName, Ref<PSO> pPso) = 0;//store the pipeline

		virtual bool AlreadyExist(std::wstring& name) = 0;

		//TODO:in the future, in terms of model name and material name to load graphics pipeline
		//virtual Ref<PSO> LoadGraphicsPipeline(std::wstring modelName, std::wstring materialName) = 0;//load graphics pipeline

		static Ref<PipelineLibrary> Create();
	};
}