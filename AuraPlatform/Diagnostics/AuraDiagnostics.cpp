#include "AuraDiagnostics.h"
#include <utility>

void AuraDiagnostics::AddSink(
    std::unique_ptr<IAuraDiagnosticSink> sink)
{
    if (!sink)
    {
        return;
    }

    sinks.push_back(std::move(sink));
}

void AuraDiagnostics::Report(
    const AuraDiagnosticEvent& event)
{
    for (const std::unique_ptr<IAuraDiagnosticSink>& sink : sinks)
    {
        if (sink)
        {
            sink->Write(event);
        }
    }
}

std::size_t AuraDiagnostics::GetSinkCount() const noexcept
{
    return sinks.size();
}