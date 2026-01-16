#pragma once
#include <string>
#include <windows.h>
#include <imgui.h>
#include <vector>

#include "Config.hpp"

namespace Utils {

	namespace Registry {
		std::string GetGamePathFromRegistry(bool isGW2);
	}

	namespace Dialog {
		std::string BrowseForExe(HWND hwnd, bool isGW2);
	}

	namespace UI {
		void CenteredInput(const char* label, std::string& value, float centerOffset, float fieldWidth, ImGuiInputTextFlags flags = 0);
		bool CenteredButton(const char* label, float width, float dpiScale);
	}

	namespace Process {
		struct LaunchResult {
			bool ok = false;
			std::string error;
			PROCESS_INFORMATION pi = { 0 };
		};

		std::string BuildArgs(const Config& cfg);
		void PatchEAArgs(const std::string& args, bool isGW2);
		void RestoreEAArgs();

		LaunchResult LaunchAndInject(const std::string& exePath, const std::string& args, const std::string& dllName = "level_loader.dll", const std::string& modDataPath = "", bool isGW2 = false);
		bool InjectDLL(DWORD processId, const std::string& dllPath);
	}

	namespace FileUtil {
		bool copy(const std::string& src, const std::string& dest, std::string& errorMessage);
		bool move(const std::string& src, const std::string& dest, std::string& errorMessage);
		bool move_replace(const std::string& src, const std::string& dest, std::string& errorMessage);
		void remove(const std::string& path);
	}
}