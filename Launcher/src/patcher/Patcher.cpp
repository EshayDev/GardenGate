#include "Patcher.hpp"
#include "Utils.hpp"

#include "xdelta3_wrapper.h"
#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

constexpr const char* BACKUP = ".bak";
constexpr const char* PATCHED = ".patched";
constexpr const char* DISABLED_DLL = ".disabled";

static bool run_xdelta(const std::string& original, const std::string& patch, const std::string& output, std::string& err) {
    std::vector<std::string> params = { "-d","-f","-s", original, patch, output };
    int result = xd3_main_exec(params);
    std::string messages = xd3_messages();

    if (result != 0) {
        err = "xdelta3 failed: " + (messages.empty()
            ? "Unknown error (code: " + std::to_string(result) + ")"
            : messages);
        return false;
    }
    return true;
}

bool Patcher::IsGW2Patched(const std::string& gamePath) {
    if (gamePath.empty() || !fs::exists(gamePath)) return false;

    std::string backupExe = gamePath + BACKUP;
    if (!fs::exists(backupExe)) return false;

    fs::path gameDir = fs::path(gamePath).parent_path();
    return fs::exists(gameDir / "dinput8.dll") ||
        fs::exists(gameDir / ("dinput8.dll" + std::string(DISABLED_DLL)));
}

bool Patcher::AutoPatchGW2(const std::string& gamePath, const std::string& patchFile, const std::string& dllFile, std::string& errorMessage)
{
    if (!fs::exists(gamePath)) { errorMessage = "Game executable not found"; return false; }
    if (!fs::exists(patchFile)) { errorMessage = "Patch file not found"; return false; }
    if (!fs::exists(dllFile)) { errorMessage = "dinput8.dll not found"; return false; }

    std::string backupExe = gamePath + BACKUP;
    Utils::FileUtil::remove(backupExe);
    if (!Utils::FileUtil::copy(gamePath, backupExe, errorMessage)) return false;

    fs::path dllTarget = fs::path(gamePath).parent_path() / "dinput8.dll";
    bool isSameFile = fs::equivalent(dllFile, dllTarget);

    if (!isSameFile) {
        if (!Utils::FileUtil::copy(dllFile, dllTarget.string(), errorMessage)) {
            if (fs::exists(dllTarget)) {
                std::string dllBackup = dllTarget.string() + ".old";
                Utils::FileUtil::move_replace(dllTarget.string(), dllBackup, errorMessage);
                Utils::FileUtil::copy(dllFile, dllTarget.string(), errorMessage);
            }
            else return false;
        }
    }

    std::string tempOutput = gamePath + PATCHED;
    if (!run_xdelta(gamePath, patchFile, tempOutput, errorMessage)) {
        Utils::FileUtil::remove(backupExe);
        Utils::FileUtil::remove(dllTarget.string());
        return false;
    }

    if (!Utils::FileUtil::move_replace(tempOutput, gamePath, errorMessage)) {
        Utils::FileUtil::remove(backupExe);
        Utils::FileUtil::remove(dllTarget.string());
        return false;
    }

    fs::path gameDir = fs::path(gamePath).parent_path();
    fs::path eaAntiCheat = gameDir / "EAAntiCheat.GameServiceLauncher.exe";
    if (fs::exists(eaAntiCheat)) {
        std::string eaDisabled = eaAntiCheat.string() + DISABLED_DLL;
        if (fs::exists(eaDisabled)) Utils::FileUtil::remove(eaDisabled);
        Utils::FileUtil::move_replace(eaAntiCheat.string(), eaDisabled, errorMessage);
    }

    fs::path installScript = gameDir / "installScript.vdf";
    if (fs::exists(installScript)) {
        std::string scriptDisabled = installScript.string() + DISABLED_DLL;
        if (fs::exists(scriptDisabled)) Utils::FileUtil::remove(scriptDisabled);
        Utils::FileUtil::move_replace(installScript.string(), scriptDisabled, errorMessage);
    }

    return true;
}

bool Patcher::RestoreGW2(const std::string& gamePath, std::string& errorMessage)
{
    if (!fs::exists(gamePath)) { errorMessage = "Game executable not found"; return false; }

    std::string backupExe = gamePath + BACKUP;
    if (!fs::exists(backupExe)) { errorMessage = "Backup EXE not found"; return false; }

    if (!Utils::FileUtil::copy(backupExe, gamePath, errorMessage)) return false;

    fs::path gameDir = fs::path(gamePath).parent_path();
    fs::path dllPath = gameDir / "dinput8.dll";
    if (fs::exists(dllPath)) {
        std::string dllDisabled = dllPath.string() + DISABLED_DLL;
        if (fs::exists(dllDisabled)) Utils::FileUtil::remove(dllDisabled);
        if (!Utils::FileUtil::move_replace(dllPath.string(), dllDisabled, errorMessage)) {
            Utils::FileUtil::remove(dllPath.string());
        }
    }

    fs::path eaAntiCheat = gameDir / "EAAntiCheat.GameServiceLauncher.exe";
    std::string eaDisabled = eaAntiCheat.string() + DISABLED_DLL;
    if (fs::exists(eaDisabled)) {
        Utils::FileUtil::move_replace(eaDisabled, eaAntiCheat.string(), errorMessage);
    }

    fs::path installScript = gameDir / "installScript.vdf";
    std::string scriptDisabled = installScript.string() + DISABLED_DLL;
    if (fs::exists(scriptDisabled)) {
        Utils::FileUtil::move_replace(scriptDisabled, installScript.string(), errorMessage);
    }

    Utils::FileUtil::remove(backupExe);
    return true;
}

bool Patcher::ApplyPatch(const std::string& originalFile, const std::string& patchFile, const std::string& outputFile,std::string& errorMessage)
{
    if (!fs::exists(originalFile)) { errorMessage = "Original file not found"; return false; }
    if (!fs::exists(patchFile)) { errorMessage = "Patch file not found"; return false; }

    return run_xdelta(originalFile, patchFile, outputFile, errorMessage);
}
