#pragma once
#include <string>
#include <string_view>
#include <utility>

// =========================================================================================
// AURA ID
// =========================================================================================

/* Universal identity value used by Aura systems.
 * Empty identity is currently permitted while foundation contracts are still being defined. */
class AuraID final
{
public:
    AuraID() = default;

    explicit AuraID(std::string value)
        : idValue(std::move(value))
    {
    }

    [[nodiscard]] std::string_view GetValue() const noexcept
    {
        return idValue;
    }

    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return idValue.empty();
    }

    [[nodiscard]] bool operator==(const AuraID& other) const noexcept
    {
        return idValue == other.idValue;
    }

    [[nodiscard]] bool operator!=(const AuraID& other) const noexcept
    {
        return !(*this == other);
    }

private:
    std::string idValue;
};