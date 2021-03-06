#pragma once

#include <string>

namespace Pixel {

	class FileDialogs
	{
	public:
		static std::wstring OpenFile(const wchar_t* filter);
		static std::wstring SaveFile(const wchar_t* filter);

	};
}