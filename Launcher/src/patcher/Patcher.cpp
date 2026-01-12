#include "Patcher.hpp"
#include "xdelta3_wrapper.h"
#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

constexpr const char* BACKUP = ".bak";
constexpr const char* PATCHED = ".patched";
constexpr const char* DISABLED_DLL = ".disabled";

namespace FileUtil {
    inline bool copy(const std::string& src, const std::string& dst, std::string& err) {
        if (!::CopyFileA(src.c_str(), dst.c_str(), FALSE)) {
            err = "Failed to copy " + src + " to " + dst;
            return false;
        }
        return true;
    }

    inline bool move(const std::string& src, const std::string& dst, std::string& err) {
        if (!::MoveFileA(src.c_str(), dst.c_str())) {
            err = "Failed to move " + src + " to " + dst;
            return false;
        }
        return true;
    }

    inline void remove(const std::string& path) {
        ::DeleteFileA(path.c_str());
    }
}

static bool run_xdelta(const std::string& original,
    const std::string& patch,
    const std::string& output,
    std::string& err)
{
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
    return fs::exists(gameDir / "dinput8.dll") || fs::exists(gameDir / ("dinput8.dll" + std::string(DISABLED_DLL)));
}

bool Patcher::AutoPatchGW2(const std::string& gamePath,
    const std::string& patchFile,
    const std::string& dllFile,
    std::string& errorMessage)
{
    if (!fs::exists(gamePath)) { errorMessage = "Game executable not found"; return false; }
    if (!fs::exists(patchFile)) { errorMessage = "Patch file not found"; return false; }
    if (!fs::exists(dllFile)) { errorMessage = "dinput8.dll not found"; return false; }

    std::string backupExe = gamePath + BACKUP;
    FileUtil::remove(backupExe);
    if (!FileUtil::copy(gamePath, backupExe, errorMessage)) return false;

    fs::path dllTarget = fs::path(gamePath).parent_path() / "dinput8.dll";
    if (!FileUtil::copy(dllFile, dllTarget.string(), errorMessage)) {
        if (fs::exists(dllTarget)) {
            std::string dllBackup = dllTarget.string() + ".old";
            FileUtil::move(dllTarget.string(), dllBackup, errorMessage);
            FileUtil::copy(dllFile, dllTarget.string(), errorMessage);
        }
        else return false;
    }

    std::string tempOutput = gamePath + PATCHED;
    if (!run_xdelta(gamePath, patchFile, tempOutput, errorMessage)) {
        FileUtil::remove(backupExe);
        FileUtil::remove(dllTarget.string());
        return false;
    }

    if (!FileUtil::move(tempOutput, gamePath, errorMessage)) {
        FileUtil::remove(backupExe);
        FileUtil::remove(dllTarget.string());
        return false;
    }

    return true;
}

bool Patcher::RestoreGW2(const std::string& gamePath,
    std::string& errorMessage)
{
    if (!fs::exists(gamePath)) { errorMessage = "Game executable not found"; return false; }

    std::string backupExe = gamePath + BACKUP;
    if (!fs::exists(backupExe)) { errorMessage = "Backup EXE not found"; return false; }

    if (!FileUtil::copy(backupExe, gamePath, errorMessage)) return false;

    fs::path dllPath = fs::path(gamePath).parent_path() / "dinput8.dll";
    if (fs::exists(dllPath)) {
        std::string dllDisabled = dllPath.string() + DISABLED_DLL;
        if (fs::exists(dllDisabled)) FileUtil::remove(dllDisabled);
        if (!FileUtil::move(dllPath.string(), dllDisabled, errorMessage)) {
            FileUtil::remove(dllPath.string());
        }
    }

    FileUtil::remove(backupExe);
    return true;
}

bool Patcher::ApplyPatch(const std::string& originalFile,
    const std::string& patchFile,
    const std::string& outputFile,
    std::string& errorMessage)
{
    if (!fs::exists(originalFile)) { errorMessage = "Original file not found"; return false; }
    if (!fs::exists(patchFile)) { errorMessage = "Patch file not found"; return false; }

    return run_xdelta(originalFile, patchFile, outputFile, errorMessage);
}