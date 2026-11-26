#pragma once
#include <string_view>

enum class AuraDiagnosticSeverity
{
    Info,
    Warning,
    Error,
    Critical
};
constexpr std::string_view ToString(
    AuraDiagnosticSeverity severity) noexcept
{
    switch (severity)
    {
    case AuraDiagnosticSeverity::Info:
        return "INFO";

    case AuraDiagnosticSeverity::Warning:
        return "WARNING";

    case AuraDiagnosticSeverity::Error:
        return "ERROR";

    case AuraDiagnosticSeverity::Critical:
        return "CRITICAL";
    }

    return "UNKNOWN";
}