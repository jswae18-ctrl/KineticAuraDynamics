#pragma once
#include "Core/AuraDiagnosticEvent.h"
#include "Sinks/IAuraDiagnosticSink.h"
#include <memory>
#include <vector>

class AuraDiagnostics final
{
public:
    AuraDiagnostics() = default;

    AuraDiagnostics(const AuraDiagnostics&) = delete;
    AuraDiagnostics& operator=(const AuraDiagnostics&) = delete;

    AuraDiagnostics(AuraDiagnostics&&) noexcept = default;
    AuraDiagnostics& operator=(AuraDiagnostics&&) noexcept = default;

    void AddSink(
        std::unique_ptr<IAuraDiagnosticSink> sink
    );

    void Report(
        const AuraDiagnosticEvent& event
    );

    std::size_t GetSinkCount() const noexcept;

private:
    std::vector<std::unique_ptr<IAuraDiagnosticSink>> sinks;
};