#pragma once

#include <exception>
#include <string>

class EngineException : public std::exception
{
public:
    EngineException(int line, const char* file);
    ~EngineException() override = default;

    const char* what() const noexcept override;
    virtual const char* GetType() const noexcept;

    const std::string& GetFile() const noexcept;
    int GetLine() const noexcept;

    std::string GetOriginString() const;

protected:
    void InvalidateWhatBuffer() const noexcept;

private:
    int line = 0;
    std::string file;

protected:
    mutable std::string whatBuffer;
};
