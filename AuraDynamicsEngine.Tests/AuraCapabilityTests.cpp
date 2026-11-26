#include "gtest/gtest.h"
#include "../AuraPlatform//Security/Capabilities/AuraCapability.h"

TEST(AuraCapabilityTests, StoresCorrectAuraID)
{
    AuraID expectedID("test.capability");

    AuraCapability capability(expectedID);

    const AuraID& actualID = capability.GetAuraCAPA_ID();

    EXPECT_EQ(actualID, expectedID);
}