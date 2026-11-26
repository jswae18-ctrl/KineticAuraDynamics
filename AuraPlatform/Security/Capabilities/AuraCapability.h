#pragma once
#include "../../../AuraPlatform//AuraIDS/CoreIDS/AuraID.h"
#include <utility>

// REPRESENTS ONE PERMISSION THAT CONTROLS WHETHER SOMETHING IS ALLOWED TO USE A PROTECTED PART OF THE AURA SYSTEM.
class AuraCapability final
{
public:
    explicit AuraCapability(AuraID id) noexcept
    : id(std::move(id))
    {}
    
    [[nodiscard]] const AuraID& GetAuraCAPA_ID() const noexcept
    {
        return id;
    }
    
private:
    AuraID id;
    
};
