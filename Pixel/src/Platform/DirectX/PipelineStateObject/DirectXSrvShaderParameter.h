#pragma once

namespace Pixel {
	class DirectXSrvShaderParameter {
	public:
		DirectXSrvShaderParameter();
	private:
		std::string m_Name;//srv name
		int32_t m_BindPoint;//srv bind point

		friend class DirectXShader;
	};
}