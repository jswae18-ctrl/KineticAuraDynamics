#include "AuraFileDiagnosticSink.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string_view>
#include <utility>

namespace
{
    std::string_view AuraErrorCodeToString(
        AuraErrorCode errorCode) noexcept
    {
        switch (errorCode)
        {
        case AuraErrorCode::None:
            return "None";

        case AuraErrorCode::InvalidIdentity:
            return "InvalidIdentity";

        case AuraErrorCode::DuplicateRegistration:
            return "DuplicateRegistration";

        case AuraErrorCode::NotFound:
            return "NotFound";

        case AuraErrorCode::Unauthorized:
            return "Unauthorized";
        }

        return "UnknownError";
    }

    std::tm GetLocalTime(std::time_t time)
    {
        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &time);
#else
        localtime_r(&time, &localTime);
#endif

        return localTime;
    }
}

AuraFileDiagnosticSink::AuraFileDiagnosticSink(
    std::filesystem::path filePath
)
    : filePath(std::move(filePath))
{
    const std::filesystem::path parentDirectory =
        this->filePath.parent_path();

    if (!parentDirectory.empty())
    {
        std::filesystem::create_directories(parentDirectory);
    }

    outputFile.open(this->filePath, std::ios::app);
}

void AuraFileDiagnosticSink::Write(
    const AuraDiagnosticEvent& event)
{
    if (!outputFile.is_open())
    {
        return;
    }

    const std::time_t eventTime =
        std::chrono::system_clock::to_time_t(
            event.GetOccurredAt()
        );

    const std::tm localTime = GetLocalTime(eventTime);

    outputFile
        << std::put_time(
            &localTime,
            "%Y-%m-%d %H:%M:%S"
        )
        << " | "
        << ToString(event.GetSeverity())
        << " | "
        << event.GetSourceId().GetValue()
        << " | "
        << event.GetEventId().GetValue()
        << " | "
        << event.GetMessage();

    if (event.GetError().has_value())
    {
        outputFile
            << " | ERROR_CODE="
            << AuraErrorCodeToString(
                event.GetError()->GetErrorCode()
            );
    }

    outputFile << '\n';
    outputFile.flush();
}

bool AuraFileDiagnosticSink::IsOpen() const noexcept
{
    return outputFile.is_open();
}

const std::filesystem::path&
AuraFileDiagnosticSink::GetFilePath() const noexcept
{
    return filePath;
}