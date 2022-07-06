#pragma once

namespace Pixel {
	enum class DepthFunc {
		LESS,
		LEQUAL
	};

	class DepthState {
	public:
		virtual ~DepthState();

		virtual void SetDepthFunc(DepthFunc func) = 0;

		virtual void DepthTest(bool enable) = 0;

		static Ref<DepthState> Create();
	};
}
