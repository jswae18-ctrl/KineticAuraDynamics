#include "AuraCommandDispatcher.h"

AuraResult<void> AuraCommandDispatcher::DispatchCommand(const AuraID& commandID,
    const AuraCommandContext& context) const
{
    if (commandID.IsEmpty())
    {
        return AuraResult<void>(AuraError(AuraErrorCode::InvalidIdentity));
    }
     const auto& foundCommand = registeredCommand.FindAuraCommand(commandID);
    if (!foundCommand.Succeeded())
    {
        return AuraResult<void>(foundCommand.GetError());
    }
    
    const auto& successfulCommand = foundCommand.GetValue();
    const AuraCommandDefinition& commandDefinition = successfulCommand.get();
    const auto& requiredCAPA = commandDefinition.GetCommandCapability();
    
    const auto& requesterID = context.GetRequester_ID();
    
    const bool isAuthorized = permissionPolicy.HasCapability(requesterID, requiredCAPA);
    
    if (!isAuthorized)
    {
        return AuraResult<void>(AuraError(AuraErrorCode::Unauthorized));
    }
    
    return AuraResult<void>{};
}
