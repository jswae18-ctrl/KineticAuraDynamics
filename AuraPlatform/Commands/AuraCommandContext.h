#pragma once
#include <utility>
#include "../../AuraPlatform/AuraIDS/CoreIDS/AuraID.h"

// Holds information about who or what is currently asking the Aura system to do something.
class AuraCommandContext final
{
public:
    explicit AuraCommandContext(AuraID id) noexcept
    : id(std::move(id))
    {}
   
    [[nodiscard]] const AuraID& GetRequester_ID() const noexcept
    {
        return id;
    }

private:
    AuraID id;
};
