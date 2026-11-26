#include "AuraDiagnosticEvent.h"

#include <utility>

AuraDiagnosticEvent::AuraDiagnosticEvent(
    AuraID eventId,
    AuraID sourceId,
    AuraDiagnosticSeverity severity,
    std::string message
)
    : eventId(std::move(eventId)),
      sourceId(std::move(sourceId)),
      severity(severity),
      message(std::move(message)),
      error(std::nullopt),
      occurredAt(std::chrono::system_clock::now())
{
}

AuraDiagnosticEvent::AuraDiagnosticEvent(
    AuraID eventId,
    AuraID sourceId,
    AuraDiagnosticSeverity severity,
    std::string message,
    AuraError error
)
    : eventId(std::move(eventId)),
      sourceId(std::move(sourceId)),
      severity(severity),
      message(std::move(message)),
      error(std::move(error)),
      occurredAt(std::chrono::system_clock::now())
{
}

const AuraID& AuraDiagnosticEvent::GetEventId() const noexcept
{
    return eventId;
}

const AuraID& AuraDiagnosticEvent::GetSourceId() const noexcept
{
    return sourceId;
}

AuraDiagnosticSeverity
AuraDiagnosticEvent::GetSeverity() const noexcept
{
    return severity;
}

const std::string&
AuraDiagnosticEvent::GetMessage() const noexcept
{
    return message;
}

const std::optional<AuraError>&
AuraDiagnosticEvent::GetError() const noexcept
{
    return error;
}

std::chrono::system_clock::time_point
AuraDiagnosticEvent::GetOccurredAt() const noexcept
{
    return occurredAt;
}