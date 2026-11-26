#pragma once
#include "AuraDiagnosticSeverity.h"
#include "../../AuraIDS/CoreIDS/AuraID.h"
#include "../../Core/Error/AuraErrors.h"
#include <chrono>
#include <optional>
#include <string>

class AuraDiagnosticEvent final
{
public:
    AuraDiagnosticEvent(
        AuraID eventId,
        AuraID sourceId,
        AuraDiagnosticSeverity severity,
        std::string message
    );

    AuraDiagnosticEvent(
        AuraID eventId,
        AuraID sourceId,
        AuraDiagnosticSeverity severity,
        std::string message,
        AuraError error
    );

    const AuraID& GetEventId() const noexcept;
    const AuraID& GetSourceId() const noexcept;

    AuraDiagnosticSeverity GetSeverity() const noexcept;

    const std::string& GetMessage() const noexcept;

    const std::optional<AuraError>& GetError() const noexcept;

    std::chrono::system_clock::time_point GetOccurredAt() const noexcept;

private:
    AuraID eventId;
    AuraID sourceId;

    AuraDiagnosticSeverity severity;

    std::string message;

    std::optional<AuraError> error;

    std::chrono::system_clock::time_point occurredAt;
};