#include <gtest/gtest.h>

#include "../AuraPlatform/Commands/AuraCommandDispatcher.h"


TEST(AuraCommandDispatcherTests, AuthorizedRequesterSucceeds)
{
    AuraID commandID("test.authorized.command");
    AuraID capabilityID("test.authorized.permission");
    AuraID requesterID("test.authorized.requester");

    AuraCapability capability(capabilityID);
    AuraCommandDefinition definition(commandID, capability);
    AuraCommandContext context(requesterID);

    AuraCommandRegistry registry;
    AuraPermissionPolicy policy;

    ASSERT_TRUE(
        registry.RegisterAuraCommand(definition).Succeeded()
    );

    ASSERT_TRUE(
        policy.GrantCapability(requesterID, capability).Succeeded()
    );

    AuraCommandDispatcher dispatcher(registry, policy);

    AuraResult<void> result =
        dispatcher.DispatchCommand(commandID, context);

    EXPECT_TRUE(result.Succeeded());
}

TEST(AuraCommandDispatcherTests, UnauthorizedRequesterIsRejected)
{
    AuraID commandID("test.unauthorized.command");
    AuraID capabilityID("test.unauthorized.permission");
    AuraID requesterID("test.unauthorized.requester");

    AuraCapability capability(capabilityID);
    AuraCommandDefinition definition(commandID, capability);
    AuraCommandContext context(requesterID);

    AuraCommandRegistry registry;
    AuraPermissionPolicy policy;

    ASSERT_TRUE(
        registry.RegisterAuraCommand(definition).Succeeded()
    );

    // Intentionally DO NOT grant the capability to this requester.

    AuraCommandDispatcher dispatcher(registry, policy);

    AuraResult<void> result =
        dispatcher.DispatchCommand(commandID, context);

    ASSERT_FALSE(result.Succeeded());

    EXPECT_EQ(
        result.GetError().GetErrorCode(),
        AuraErrorCode::Unauthorized
    );
}


TEST(AuraCommandDispatcherTests, MissingCommandReturnsNotFound)
{
    AuraID commandID("test.missing.command");
    AuraID requesterID("test.missing.requester");

    AuraCommandContext context(requesterID);

    AuraCommandRegistry registry;
    AuraPermissionPolicy policy;

    AuraCommandDispatcher dispatcher(registry, policy);

    AuraResult<void> result =
        dispatcher.DispatchCommand(commandID, context);

    ASSERT_FALSE(result.Succeeded());

    EXPECT_EQ(
        result.GetError().GetErrorCode(),
        AuraErrorCode::NotFound
    );
}