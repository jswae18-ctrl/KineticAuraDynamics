#include "AuraCommandRegistry.h"
#include <utility>

AuraResult<void> AuraCommandRegistry::RegisterAuraCommand(AuraCommandDefinition definition)
{
    AuraID commandId = definition.GetCommand_ID();
    
    if (commandId.IsEmpty())
    {
        return AuraResult<void>(AuraError(AuraErrorCode::InvalidIdentity));
    }
    
    
    if (registryCommands.find(commandId) != registryCommands.end())
    {
        return AuraResult<void>(AuraError(AuraErrorCode::DuplicateRegistration));
    }
    
    registryCommands.emplace(std::move(commandId), std::move(definition));
    
    return AuraResult<void>{};
}

AuraResult<std::reference_wrapper<const AuraCommandDefinition>> AuraCommandRegistry::FindAuraCommand(
    const AuraID& commandID) const
{
    if (commandID.IsEmpty())
    {
        return AuraResult<std::reference_wrapper<const AuraCommandDefinition>>(AuraError(AuraErrorCode::InvalidIdentity));
    }
    auto command = registryCommands.find(commandID);
    if (command == registryCommands.end())
    {
       return AuraResult<std::reference_wrapper<const AuraCommandDefinition>>(AuraError(AuraErrorCode::NotFound));
    }
    
    // RETURNS: std::cref(command.>second)
    return AuraResult<std::reference_wrapper<const AuraCommandDefinition>>(command->second);
}



