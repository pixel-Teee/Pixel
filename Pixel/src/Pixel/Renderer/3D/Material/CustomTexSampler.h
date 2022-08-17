//------my library------
#include "MaterialCustomPara.h"
#include "Pixel/Renderer/Sampler/Sampler.h"
//------my library------

namespace Pixel {
	class CustomTexSampler : public MaterialCustomPara
	{
	public:
		CustomTexSampler();

		virtual ~CustomTexSampler();

		//sampler state
		//TODO:Sampler Desc will renamed SamplerState
		Ref<SamplerDesc> m_SamplerState;

		CustomTexSampler& operator=(const CustomTexSampler& rhs)
		{
			//assign name
			MaterialCustomPara::operator=(rhs);
			//TODO:is it required clone function?
			//m_SamplerState = rhs.m_SamplerState;
			m_SamplerState = rhs.m_SamplerState->Clone();
			return *this;
		}

		friend bool operator==(const CustomTexSampler& lhs, const CustomTexSampler& rhs)
		{
			return lhs.ConstValueName == rhs.ConstValueName;
		}
	};
}