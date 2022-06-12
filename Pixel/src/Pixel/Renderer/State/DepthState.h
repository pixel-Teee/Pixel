#pragma once

namespace Pixel {
	class DepthState {
	public:
		virtual ~DepthState();

		static Ref<DepthState> Create();
	};
}
