#pragma once

#include "Pixel/Renderer/BaseRenderer.h"

namespace Pixel {
	class BufferLayout;
	class PSO;
	class Device;
	class RootSignature;
	class DirectXRenderer : public BaseRenderer
	{
	public:
		DirectXRenderer(Ref<Device> pDevice);
		virtual ~DirectXRenderer();

		virtual void CreatePso(BufferLayout& layout) override;
	private:
		Ref<Device> m_pDevice;

		Ref<RootSignature> m_rootSignature;

		std::vector<Ref<PSO>> m_PsoArray;

		Ref<PSO> m_defaultPso;
	};
}
