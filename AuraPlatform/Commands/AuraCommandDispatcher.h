#pragma once
#include "AuraCommandContext.h"
#include "../../AuraPlatform/Commands/AuraCommandRegistry.h"
#include "../../AuraPlatform/Security/Authorization/AuraPermissionPolicy.h"

class AuraCommandDispatcher final
{
public:
    AuraCommandDispatcher(const AuraCommandRegistry& registry, const AuraPermissionPolicy& policy) noexcept
    : registeredCommand(registry), permissionPolicy(policy)
    {}
    
    AuraResult<void> DispatchCommand(const AuraID& commandID, const AuraCommandContext& context) const;

private:
    const AuraCommandRegistry& registeredCommand;
    const AuraPermissionPolicy& permissionPolicy;
    
    
};
