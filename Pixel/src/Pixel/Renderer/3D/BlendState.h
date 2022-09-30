#pragma once

namespace Pixel {
	class BlendState {
	public:
		enum BlendFactor {
			ZERO,
			ONE,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA
		};
		BlendState();
		virtual ~BlendState();
	public:
		//FinalColor = SrcAlpha * SrcColor + (1 - OneMinusSrcAlpha) * DestColor
		BlendFactor m_SrcColor;
		BlendFactor m_DestColor;
	};
}