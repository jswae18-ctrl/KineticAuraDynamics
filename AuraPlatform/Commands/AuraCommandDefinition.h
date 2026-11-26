#pragma once
#include <utility>
#include "../../AuraPlatform/AuraIDS/CoreIDS/AuraID.h"
#include "../../AuraPlatform/Security/Capabilities/AuraCapability.h"

// DESCRIBES ONE OPERATION AURA KNOWS ABOUT AND THE PERMISSION REQUIRED TO USE IT.
class AuraCommandDefinition final
{
public:
    explicit AuraCommandDefinition(AuraID id, AuraCapability requiredCapability)
        : id(std::move(id)), requiredCapability(std::move(requiredCapability))
    {}
    
    [[nodiscard]] const AuraID& GetCommand_ID() const noexcept
    {
        return id;
    }
    
    [[nodiscard]] const AuraCapability& GetCommandCapability() const noexcept
    {
        return requiredCapability;
    }
private:
    AuraID id;
    AuraCapability requiredCapability;
};
