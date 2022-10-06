#include "pxpch.h"
#include "Pixel/Utils/PlatformUtils.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Pixel/Core/Application.h"

namespace Pixel {

	std::wstring FileDialogs::OpenFile(const wchar_t* filter)
	{
		OPENFILENAME ofn;
		WCHAR szFile[260] = {0};

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::wstring();
	}

	std::wstring FileDialogs::SaveFile(const wchar_t* filter)
	{
		OPENFILENAME ofn;
		WCHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileName(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::wstring();
	}

	std::wstring FileDialogs::OpenFile(const wchar_t* filter, const wchar_t* initialDir)
	{
		OPENFILENAME ofn;
		WCHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = initialDir;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::wstring();
	}

	std::wstring FileDialogs::SaveFile(const wchar_t* filter, const wchar_t* initialDir)
	{
		OPENFILENAME ofn;
		WCHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = initialDir;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileName(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::wstring();
	}

}