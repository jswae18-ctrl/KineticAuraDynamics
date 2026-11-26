#pragma once
#include <unordered_map>
#include <unordered_set>
#include "../../../AuraPlatform/AuraIDS/CoreIDS/AuraID.h"
#include "../../../AuraPlatform/AuraIDS/CoreIDS/AuraHasher.h"
#include "../../../AuraPlatform/Security/Capabilities/AuraCapability.h"
#include "../../../AuraPlatform/Core/Result/AuraResults.h"

// GRANTS PERMISSION TO ALL MAIN AURA REGISTERED SYSTEMS.
class AuraPermissionPolicy final
{
public:
    // GRANTS THE REQUESTER, AURA CAPABILITY
  AuraResult<void> GrantCapability(const AuraID& requester_ID, const AuraCapability& capability);
    
    bool HasCapability(const AuraID& requester_ID, const AuraCapability& capability) const noexcept;
private:
    // AuraID->first; std::unordered_set<AuraID, AuraHasher>->second
    std::unordered_map<AuraID, std::unordered_set<AuraID, AuraHasher>, AuraHasher> permissions;
};
