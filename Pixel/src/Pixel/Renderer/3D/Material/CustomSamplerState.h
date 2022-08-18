#pragma once
//------my library------
#include "MaterialCustomPara.h"
#include "Pixel/Renderer/Sampler/Sampler.h"
//------my library------

namespace Pixel {
	class CustomSamplerState : public MaterialCustomPara
	{
	public:
		CustomSamplerState();

		virtual ~CustomSamplerState();

		//sampler state
		//TODO:Sampler Desc will renamed SamplerState
		Ref<SamplerDesc> m_SamplerState;

		CustomSamplerState& operator=(const CustomSamplerState& rhs)
		{
			//assign name
			MaterialCustomPara::operator=(rhs);
			//TODO:is it required clone function?
			//m_SamplerState = rhs.m_SamplerState;
			m_SamplerState = rhs.m_SamplerState->Clone();
			return *this;
		}

		friend bool operator==(const CustomSamplerState& lhs, const CustomSamplerState& rhs)
		{
			return lhs.ConstValueName == rhs.ConstValueName;
		}
	};
}