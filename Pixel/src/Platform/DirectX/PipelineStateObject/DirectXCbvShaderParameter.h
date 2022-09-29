#pragma once

namespace Pixel {
	struct CbvVariableParameter
	{
		std::string m_VariableName;
		int32_t m_StartOffset;
		int32_t m_Size;
	};

	class DirectXCbvShaderParameter {
	public:
		DirectXCbvShaderParameter();
	private:
		std::string m_Name;//const buffer name
		int32_t m_Size;//const buffer size, will in terms to create 256 bytes size buffer
		int32_t m_BindPoint;//const buffer bind point

		std::vector<CbvVariableParameter> m_CbvVariableParameters;//constant buffer variable parameters
		friend class DirectXShader;
	};
}