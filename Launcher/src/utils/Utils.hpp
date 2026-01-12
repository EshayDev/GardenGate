#pragma once
#include <string>
#include <windows.h>
#include <imgui.h>
#include <vector>

namespace Utils {

    namespace Registry {
        std::string GetGamePathFromRegistry(bool isGW2);
    }

    namespace Dialog {
        std::string BrowseForExe(HWND hwnd, bool isGW2);
    }

    namespace UI {
        void CenteredInput(const char* label, std::string& value,
            float centerOffset, float fieldWidth,
            ImGuiInputTextFlags flags = 0);

        bool CenteredButton(const char* label, float width, float dpiScale);

        struct Status {
            std::string message;
            ImVec4 color = ImVec4(1, 1, 1, 1);
            bool visible = false;
            float timer = 0.0f;

            void Show(const std::string& msg, bool error = false);
            void Update(float dt);
        };
    }

    namespace Process {
        bool LaunchGame(const std::string& exePath,
            const std::string& args,
            HWND hwnd,
            std::string& errorMessage);
    }
}
