#pragma once
#include "../../AuraPlatform/Commands/AuraCommandDefinition.h"
#include "../../AuraPlatform/AuraIDS/CoreIDS/AuraHasher.h"
#include "../../AuraPlatform/Core/Result/AuraResults.h"
#include <unordered_map>
#include <functional> // Required for std::reference_wrapper and std::cref

// STORES THE THINGS AURA IS ALLOWED TO RECOGNIZE AS KNOWN OPERATIONS AND LETS THE SYSTEM FIND THEM BY IDENTITY.
class AuraCommandRegistry final
{
public:
    AuraResult<void> RegisterAuraCommand(AuraCommandDefinition definition);
    AuraResult<std::reference_wrapper<const AuraCommandDefinition>> FindAuraCommand(const AuraID& commandID) const;
private:
    std::unordered_map<AuraID, AuraCommandDefinition, AuraHasher> registryCommands;
    
    
};
