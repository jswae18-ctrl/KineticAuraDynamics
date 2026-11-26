#pragma once
#include <cstdint>

struct AuraSchemaVersion final
{
    std::uint32_t value = 0;
    
    [[nodiscard]] bool operator==(const AuraSchemaVersion& other) const noexcept
    {
        return value == other.value;
    }
    
    [[nodiscard]] bool operator!=(const AuraSchemaVersion& other) const noexcept
    {
        return !(*this == other);
    }
};
