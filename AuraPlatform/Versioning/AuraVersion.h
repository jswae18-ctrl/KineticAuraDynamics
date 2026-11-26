#pragma once
#include <cstdint>
struct AuraVersion final
{
    std::uint32_t major = 0;
    std::uint32_t minor = 0;
    std::uint32_t patch = 0;
    
    [[nodiscard]] bool operator==(const AuraVersion& other) const noexcept
    {
        return 
        major == other.major &&
        minor == other.minor &&
        patch == other.patch;
    }
    
    [[nodiscard]] bool operator!=(const AuraVersion& other) const noexcept
    {
        return !(*this == other);
    }
};