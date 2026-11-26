#include "AuraPermissionPolicy.h"

AuraResult<void> AuraPermissionPolicy::GrantCapability(const AuraID& requester_ID, const AuraCapability& capability)
{
    if (requester_ID.IsEmpty())
    {
        return AuraResult<void>(AuraError(AuraErrorCode::InvalidIdentity));
    }
    AuraID CAPA_ID = capability.GetAuraCAPA_ID();
    
    if (CAPA_ID.IsEmpty())
    {
        return AuraResult<void>(AuraError(AuraErrorCode::InvalidIdentity));
    }
    
    auto& requesterPermission = permissions[requester_ID];
    requesterPermission.insert(CAPA_ID);
    
    return AuraResult<void>{};
}

bool AuraPermissionPolicy::HasCapability(const AuraID& requester_ID, const AuraCapability& capability) const noexcept
{
    const auto& requester = permissions.find(requester_ID);
    if (requester == permissions.end())
    {
        return false;
    }
    
    auto const requesterCAPA = requester->second;
    const AuraID& CAPA_ID = capability.GetAuraCAPA_ID();
    
    return requesterCAPA.find(CAPA_ID) != requesterCAPA.end();
}