#include "EngineException.h"

#include <sstream>

namespace
{
    constexpr const char* UnknownSourceFile = "Unknown source file";
    constexpr const char* ExceptionFormattingFailure =
        "EngineException: failed to format exception details.";
}

EngineException::EngineException(int line, const char* file)
    :
    line(line),
    file(file != nullptr ? file : UnknownSourceFile)
{
}

const char* EngineException::what() const noexcept
{
    if (!whatBuffer.empty())
    {
        return whatBuffer.c_str();
    }

    try
    {
        std::ostringstream stream;

        stream << GetType() << "\n";
        stream << GetOriginString();

        whatBuffer = stream.str();

        return whatBuffer.c_str();
    }
    catch (...)
    {
        return ExceptionFormattingFailure;
    }
}

const char* EngineException::GetType() const noexcept
{
    return "EngineException";
}

const std::string& EngineException::GetFile() const noexcept
{
    return file;
}

int EngineException::GetLine() const noexcept
{
    return line;
}

std::string EngineException::GetOriginString() const
{
    std::ostringstream stream;

    stream << "[FILE] " << file << "\n";
    stream << "[LINE] " << line;

    return stream.str();
}

void EngineException::InvalidateWhatBuffer() const noexcept
{
    whatBuffer.clear();
}