#include "AuraDynamicsDiagnostics.h"
#include "../../AuraPlatform/Diagnostics/Sinks/AuraFileDiagnosticSink.h"
#include <memory>
#include <utility>

AuraDynamicsDiagnostics::AuraDynamicsDiagnostics(
    std::filesystem::path logFilePath)
{
    diagnostics.AddSink(
        std::make_unique<AuraFileDiagnosticSink>(
            std::move(logFilePath)
        )
    );
}

AuraDiagnostics&
AuraDynamicsDiagnostics::GetDiagnostics() noexcept
{
    return diagnostics;
}

const AuraDiagnostics&
AuraDynamicsDiagnostics::GetDiagnostics() const noexcept
{
    return diagnostics;
}