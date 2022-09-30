#pragma once

namespace Pixel {
	class DirectXSrvShaderParameter {
	public:
		DirectXSrvShaderParameter();
	private:
		std::string m_Name;//srv name
		int32_t m_BindPoint;//srv bind point
		int32_t m_RootIndex = -1;//srv root index
		int32_t m_Offset = -1;//from descriptor range offset

		friend class DirectXShader;
		friend class DirectXRenderer;
	};
}