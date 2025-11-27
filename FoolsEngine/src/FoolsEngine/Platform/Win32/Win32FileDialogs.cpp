#include "FE_pch.h"
#ifdef FE_PLATFORM_WINDOWS

#include "FoolsEngine/Platform/FileDialogs.h"

#include <filesystem>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "FoolsEngine/Core/Application.h"
#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe
{
	std::filesystem::path FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().m_Window->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			SetCurrentDirectoryA(currentDir);
			return std::filesystem::relative(ofn.lpstrFile, currentDir);
		}
		return std::filesystem::path();
	}

	//defaultpath should be absolute
	std::filesystem::path FileDialogs::SaveFile(const char* defaultpath, const char* filter)
	{
		Scratchpad sp;

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		strncpy_s(szFile, defaultpath, sizeof(szFile));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().m_Window->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		CHAR currentDir[256] = { 0 };
		auto def_path = std::filesystem::path(defaultpath).parent_path().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
		strncpy_s(currentDir, def_path.c_str(), sizeof(currentDir));
		ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		return std::filesystem::path();
	}
}

#endif