#pragma once
#include <string>

class Patcher {
public:
    static bool IsGW2Patched(const std::string& gamePath);

    static bool AutoPatchGW2(const std::string& gamePath,
        const std::string& patchFile,
        const std::string& dllFile,
        std::string& errorMessage);

    static bool RestoreGW2(const std::string& gamePath,
        std::string& errorMessage);

    static bool ApplyPatch(const std::string& originalFile,
        const std::string& patchFile,
        const std::string& outputFile,
        std::string& errorMessage);
};