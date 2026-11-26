#pragma once
#include "../../AuraPlatform/Diagnostics/AuraDiagnostics.h"
#include <filesystem>

class AuraDynamicsDiagnostics final
{
public:
    explicit AuraDynamicsDiagnostics(
        std::filesystem::path logFilePath
    );

    AuraDiagnostics& GetDiagnostics() noexcept;

    const AuraDiagnostics&
    GetDiagnostics() const noexcept;

private:
    AuraDiagnostics diagnostics;
};