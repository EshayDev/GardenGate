#include "Utils.hpp"
#include <filesystem>
#include <ShlObj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <process.h>

namespace fs = std::filesystem;

namespace Utils {

    namespace Registry {
        std::string GetGamePathFromRegistry(bool isGW2) {
            HKEY hKey = NULL;
            LONG result = 0;
            char pathBuffer[MAX_PATH] = { 0 };
            DWORD bufferSize = MAX_PATH;

            std::string regPath;
            if (isGW2) {
                regPath = "SOFTWARE\\WOW6432Node\\PopCap\\Plants vs Zombies GW2";
                result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey);
                if (result != ERROR_SUCCESS) {
                    regPath = "SOFTWARE\\PopCap\\Plants vs Zombies GW2";
                    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey);
                }
            }
            else {
                regPath = "SOFTWARE\\WOW6432Node\\PopCap\\Plants vs Zombies Garden Warfare";
                result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey);
                if (result != ERROR_SUCCESS) {
                    regPath = "SOFTWARE\\PopCap\\Plants vs Zombies Garden Warfare";
                    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey);
                }
            }

            if (result == ERROR_SUCCESS) {
                result = RegQueryValueExA(hKey, "Install Dir", NULL, NULL, (LPBYTE)pathBuffer, &bufferSize);
                if (result != ERROR_SUCCESS) {
                    result = RegQueryValueExA(hKey, "Path", NULL, NULL, (LPBYTE)pathBuffer, &bufferSize);
                }
                RegCloseKey(hKey);
            }
            return "";
        }
    }

    namespace Dialog {
        std::string BrowseForExe(HWND hwnd, bool isGW2) {
            char pathBuffer[MAX_PATH] = "";
            OPENFILENAMEA ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = isGW2 ?
                "GW2 Executable\0GW2.Main_Win64_Retail.exe;*.exe\0All Files\0*.*\0" :
                "GW1 Executable\0PVZ.Main_Win64_Retail.exe;*.exe\0All Files\0*.*\0";
            ofn.lpstrFile = pathBuffer;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileNameA(&ofn)) return pathBuffer;
            return "";
        }
    }

    namespace UI {
        void CenteredInput(const char* label, std::string& value,
            float centerOffset, float fieldWidth,
            ImGuiInputTextFlags flags) {
            ImGui::SetCursorPosX(centerOffset);
            ImGui::TextUnformatted(label);
            ImGui::SetCursorPosX(centerOffset);
            ImGui::SetNextItemWidth(fieldWidth);

            char buf[256];
            strncpy_s(buf, value.c_str(), sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            if (ImGui::InputText(std::string("##").append(label).c_str(), buf, sizeof(buf), flags)) {
                value = buf;
                if (strcmp(label, "Username:") == 0 && value.length() > 15) value.resize(15);
            }
        }

        bool CenteredButton(const char* label, float width, float dpiScale) {
            float safeWidth = ImGui::GetContentRegionAvail().x;
            float offset = (safeWidth - width) / 2.0f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            return ImGui::Button(label, ImVec2(width, 38.0f * dpiScale));
        }

        void Status::Show(const std::string& msg, bool error) {
            message = msg;
            color = error ? ImVec4(1, 0.5f, 0.5f, 1) : ImVec4(0.5f, 1, 0.5f, 1);
            visible = true;
            timer = 0.0f;
        }

        void Status::Update(float dt) {
            if (visible) {
                timer += dt;
                if (timer > 3.0f) visible = false;
            }
        }
    }

    namespace Process {
        bool LaunchGame(const std::string& exePath,
            const std::string& args,
            HWND hwnd,
            std::string& errorMessage) {
            if (exePath.empty() || !fs::exists(exePath)) {
                errorMessage = "Invalid game path.";
                return false;
            }

            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi = { 0 };
            std::string cmd = "\"" + exePath + "\" " + args;
            std::vector<char> cmdLine(cmd.begin(), cmd.end());
            cmdLine.push_back('\0');

            BOOL success = CreateProcessA(NULL, cmdLine.data(), NULL, NULL, FALSE, 0, NULL,
                fs::path(exePath).parent_path().string().c_str(),
                &si, &pi);
            if (success) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                return true;
            }
            else {
                errorMessage = "Failed to launch game. Error code: " + std::to_string(GetLastError());
                return false;
            }
        }
    }
}
