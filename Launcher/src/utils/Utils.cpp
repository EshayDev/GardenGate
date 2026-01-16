#include "Utils.hpp"

#include <fstream> 
#include <filesystem>
#include <map>
#include <algorithm>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <process.h>
#include <tlhelp32.h>

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
				return std::string(pathBuffer);
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
	}

	namespace Process {

		std::string BuildArgs(const Config& cfg) {
			const GameConfig& game = cfg.get_current_game();
			std::vector<std::string> argList;

			if (!game.custom_args.empty()) argList.push_back(game.custom_args);

			if (!cfg.username.empty()) argList.push_back("-name " + cfg.username);
			if (!cfg.server_ip.empty()) argList.push_back("-Client.ServerIp " + cfg.server_ip);
			if (!cfg.password.empty()) argList.push_back("-Server.ServerPassword " + cfg.password);

			std::string args;
			for (size_t i = 0; i < argList.size(); ++i) {
				if (i) args += ' ';
				args += argList[i];
			}

			return args;
		}

		static std::map<std::string, std::string> originalArgs;

		void PatchEAArgs(const std::string& args, bool isGW2) {
			fs::path eaDir = fs::path(getenv("LOCALAPPDATA")) / "Electronic Arts" / "EA Desktop";

			std::string cleanedArgs = args;
			if (!cleanedArgs.empty()) {
				cleanedArgs.erase(std::remove(cleanedArgs.begin(), cleanedArgs.end(), '\"'), cleanedArgs.end());
			}

			for (const auto& entry : fs::directory_iterator(eaDir)) {
				if (entry.is_regular_file() && entry.path().extension() == ".ini") {
					std::ifstream in(entry.path());
					if (!in) continue;

					std::string line, output;
					bool modified = false;

					while (std::getline(in, line)) {
						if (line.rfind("user.gamecommandline.ofb-east:", 0) == 0) {
							auto pos = line.find('=');
							if (pos != std::string::npos) {
								if (originalArgs.find(entry.path().string()) == originalArgs.end()) {
									originalArgs[entry.path().string()] = line.substr(pos + 1);
								}
								line = line.substr(0, pos + 1) + cleanedArgs;
								modified = true;
							}
						}
						output += line + "\n";
					}
					in.close();

					if (modified) {
						std::ofstream out(entry.path(), std::ios::trunc);
						out << output;
					}
				}
			}
		}

		void RestoreEAArgs() {
			for (const auto& [filePath, originalValue] : originalArgs) {
				std::ifstream in(filePath);
				if (!in) continue;

				std::string line, output;
				bool modified = false;

				while (std::getline(in, line)) {
					if (line.rfind("user.gamecommandline.ofb-east:", 0) == 0) {
						auto pos = line.find('=');
						if (pos != std::string::npos) {
							line = line.substr(0, pos + 1) + originalValue;
							modified = true;
						}
					}
					output += line + "\n";
				}
				in.close();

				if (modified) {
					std::ofstream out(filePath, std::ios::trunc);
					out << output;
				}
			}
			originalArgs.clear();
		}

		bool InjectDLL(DWORD processId, const std::string& dllPath) {
			HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
				PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
				FALSE, processId);
			if (!hProcess) return false;

			char fullPath[MAX_PATH];
			if (!GetFullPathNameA(dllPath.c_str(), MAX_PATH, fullPath, nullptr)) {
				CloseHandle(hProcess);
				return false;
			}

			SIZE_T bytes = strlen(fullPath) + 1;
			LPVOID remoteMem = VirtualAllocEx(hProcess, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!remoteMem) {
				CloseHandle(hProcess);
				return false;
			}

			if (!WriteProcessMemory(hProcess, remoteMem, fullPath, bytes, nullptr)) {
				VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
				CloseHandle(hProcess);
				return false;
			}

			LPTHREAD_START_ROUTINE loadLibAddr =
				(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
			if (!loadLibAddr) {
				VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
				CloseHandle(hProcess);
				return false;
			}

			HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibAddr, remoteMem, 0, nullptr);
			if (!hThread) {
				VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
				CloseHandle(hProcess);
				return false;
			}

			WaitForSingleObject(hThread, INFINITE);

			CloseHandle(hThread);
			VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
			CloseHandle(hProcess);
			return true;
		}

		void KillProcessByName(const std::string& processName) {
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE) return;

			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(snapshot, &pe32)) {
				do {
					if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
						HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
						if (hProcess) {
							TerminateProcess(hProcess, 0);
							CloseHandle(hProcess);
						}
					}
				} while (Process32Next(snapshot, &pe32));
			}

			CloseHandle(snapshot);
		}

		LaunchResult LaunchAndInject(const std::string& exePath,
			const std::string& args,
			const std::string& dllName,
			const std::string& modDataPath,
			bool isGW2)
		{
			KillProcessByName("EADesktop.exe");
			KillProcessByName("Origin.exe");
			
			if (isGW2) {
				KillProcessByName("steam.exe");
			}

			LaunchResult lr;
			STARTUPINFOA si = { sizeof(si) };
			PROCESS_INFORMATION pi{};

			std::string cmdLineStr = "\"" + exePath + "\" " + args;
			std::vector<char> cmdLine(cmdLineStr.begin(), cmdLineStr.end());
			cmdLine.push_back('\0');

			LPVOID envBlock = nullptr;
			if (!modDataPath.empty()) {
				std::string envVar = "GAME_DATA_DIR=" + modDataPath;
				std::string envStr;
				
				LPCH envStrings = GetEnvironmentStringsA();
				if (envStrings) {
					LPCH p = envStrings;
					while (*p) {
						envStr += p;
						envStr += '\0';
						p += strlen(p) + 1;
					}
					FreeEnvironmentStringsA(envStrings);
				}
				
				envStr += envVar;
				envStr += '\0';
				envStr += '\0';
				
				std::vector<char> envBuffer(envStr.begin(), envStr.end());
				envBlock = envBuffer.data();
				
				BOOL ok = CreateProcessA(
					nullptr, cmdLine.data(),
					nullptr, nullptr, FALSE, 0, envBlock,
					fs::path(exePath).parent_path().string().c_str(),
					&si, &pi);

				if (!ok) {
					lr.error = "CreateProcess failed. Error: " + std::to_string(GetLastError());
					return lr;
				}
			} else {
				BOOL ok = CreateProcessA(
					nullptr, cmdLine.data(),
					nullptr, nullptr, FALSE, 0, nullptr,
					fs::path(exePath).parent_path().string().c_str(),
					&si, &pi);

				if (!ok) {
					lr.error = "CreateProcess failed. Error: " + std::to_string(GetLastError());
					return lr;
				}
			}

			lr.ok = true;
			lr.pi = pi;

			if (!isGW2) {
				fs::path dllPath = fs::path(exePath).parent_path() / dllName;
				if (!fs::exists(dllPath)) {
					lr.error = dllName + " not found next to the game exe.";
					lr.ok = false;
					return lr;
				}

				const wchar_t* Gw1Title = L"PVZ Garden Warfare";
				DWORD pid = 0;
				int waited = 0;
				while (waited < 30000) {
					HWND hwnd = FindWindowW(nullptr, Gw1Title);
					if (hwnd) {
						GetWindowThreadProcessId(hwnd, &pid);
						if (pid) break;
					}
					Sleep(500);
					waited += 500;
				}

				if (!pid) {
					lr.error = "Timed out waiting for GW1 window.";
					lr.ok = false;
					return lr;
				}

				Sleep(4000);

				if (!InjectDLL(pid, dllPath.string())) {
					lr.error = "DLL injection into GW1 failed.";
					lr.ok = false;
					return lr;
				}
			}

			return lr;
		}
	}

	namespace FileUtil {
		bool copy(const std::string& src, const std::string& dst, std::string& err) {
			if (!::CopyFileA(src.c_str(), dst.c_str(), FALSE)) {
				err = "Failed to copy " + src + " to " + dst;
				return false;
			}
			return true;
		}

		bool move(const std::string& src, const std::string& dst, std::string& err) {
			if (!::MoveFileA(src.c_str(), dst.c_str())) {
				err = "Failed to move " + src + " to " + dst;
				return false;
			}
			return true;
		}

		bool move_replace(const std::string& src, const std::string& dst, std::string& err) {
			if (!::MoveFileExA(src.c_str(), dst.c_str(), MOVEFILE_REPLACE_EXISTING)) {
				err = "Failed to replace " + dst + " with " + src;
				return false;
			}
			return true;
		}

		void remove(const std::string& path) {
			::DeleteFileA(path.c_str());
		}
	}
}