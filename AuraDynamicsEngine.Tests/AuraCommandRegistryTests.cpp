#include <gtest/gtest.h>

#include "../AuraPlatform/Commands/AuraCommandRegistry.h"
#include "../AuraPlatform/AuraIDS/CoreIDS/AuraID.h"


TEST(AuraCommandRegistryTests, RegistryStoresCommand)
{
    // ARRANGE
    AuraID commandID("test.commandRegistry");
    AuraID capabilityID("test.commandRegistry.capability");

    AuraCapability capability(capabilityID);

    AuraCommandDefinition definition(
        commandID,
        capability
    );

    AuraCommandRegistry registry;


    // ACT
    AuraResult<void> registerResult =
        registry.RegisterAuraCommand(definition);

    const auto findResult =
        registry.FindAuraCommand(commandID);


    // ASSERT
    EXPECT_TRUE(registerResult.Succeeded());

    EXPECT_TRUE(findResult.Succeeded());

    ASSERT_TRUE(findResult.Succeeded());

    const AuraCommandDefinition& foundDefinition =
        findResult.GetValue().get();

    EXPECT_EQ(
        foundDefinition.GetCommand_ID(),
        commandID
    );

    EXPECT_EQ(
        foundDefinition.GetCommandCapability().GetAuraCAPA_ID(),
        capabilityID
    );
}