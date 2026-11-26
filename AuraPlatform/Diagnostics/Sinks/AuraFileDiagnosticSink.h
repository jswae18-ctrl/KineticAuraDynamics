#pragma once
#include "IAuraDiagnosticSink.h"
#include <filesystem>
#include <fstream>

class AuraFileDiagnosticSink final : public IAuraDiagnosticSink
{
public:
    explicit AuraFileDiagnosticSink(
        std::filesystem::path filePath
    );

    void Write(
        const AuraDiagnosticEvent& event
    ) override;

    bool IsOpen() const noexcept;

    const std::filesystem::path&
    GetFilePath() const noexcept;

private:
    std::filesystem::path filePath;
    std::ofstream outputFile;
};