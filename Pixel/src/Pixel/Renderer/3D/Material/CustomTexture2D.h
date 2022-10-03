#pragma once
//------my library------
#include "MaterialCustomPara.h"
#include "Pixel/Renderer/Sampler/Sampler.h"
//------my library------

namespace Pixel {
	class Texture2D;
	class Meta(Enable) CustomTexture2D : public MaterialCustomPara
	{
	public:
		Meta()
		CustomTexture2D();

		virtual ~CustomTexture2D();

		//sampler state
		//TODO:Sampler Desc will renamed SamplerState
		//Ref<SamplerDesc> m_SamplerState;

		CustomTexture2D& operator=(const CustomTexture2D& rhs)
		{
			//assign name
			MaterialCustomPara::operator=(rhs);
			//TODO:is it required clone function?
			//m_SamplerState = rhs.m_SamplerState;
			//m_SamplerState = rhs.m_SamplerState->Clone();
			return *this;
		}

		friend bool operator==(const CustomTexture2D& lhs, const CustomTexture2D& rhs)
		{
			return lhs.ConstValueName == rhs.ConstValueName;
		}

		Ref<Texture2D> m_pTexture;

		RTTR_ENABLE(MaterialCustomPara)
		RTTR_REGISTRATION_FRIEND
	};
}