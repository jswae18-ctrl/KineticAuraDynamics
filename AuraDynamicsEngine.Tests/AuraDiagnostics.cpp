#include <gtest/gtest.h>

#include "../AuraPlatform/Diagnostics/AuraDiagnostics.h"
#include "../AuraPlatform/Diagnostics/Sinks/AuraFileDiagnosticSink.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>


TEST(AuraDiagnosticsTests, EventStoresDiagnosticInformation)
{
    AuraDiagnosticEvent event(
        AuraID("test.event"),
        AuraID("test.source"),
        AuraDiagnosticSeverity::Warning,
        "Something unusual happened."
    );

    EXPECT_EQ(event.GetEventId(), AuraID("test.event"));
    EXPECT_EQ(event.GetSourceId(), AuraID("test.source"));
    EXPECT_EQ(event.GetSeverity(), AuraDiagnosticSeverity::Warning);
    EXPECT_EQ(event.GetMessage(), "Something unusual happened.");

    EXPECT_FALSE(event.GetError().has_value());
}


TEST(AuraDiagnosticsTests, EventCanStoreAuraError)
{
    AuraDiagnosticEvent event(
        AuraID("test.error.event"),
        AuraID("test.error.source"),
        AuraDiagnosticSeverity::Error,
        "Command could not be found.",
        AuraError(AuraErrorCode::NotFound)
    );

    ASSERT_TRUE(event.GetError().has_value());

    EXPECT_EQ(
        event.GetError()->GetErrorCode(),
        AuraErrorCode::NotFound
    );
}


TEST(AuraDiagnosticsTests, DiagnosticsRoutesEventToFileSink)
{
    const std::filesystem::path logPath =
        "AuraDiagnosticsTest.log";

    std::filesystem::remove(logPath);

    {
        AuraDiagnostics diagnostics;

        auto fileSink =
            std::make_unique<AuraFileDiagnosticSink>(logPath);

        ASSERT_TRUE(fileSink->IsOpen());

        diagnostics.AddSink(std::move(fileSink));

        ASSERT_EQ(diagnostics.GetSinkCount(), 1u);

        AuraDiagnosticEvent event(
            AuraID("test.routing.event"),
            AuraID("aura.test"),
            AuraDiagnosticSeverity::Error,
            "Diagnostic routing test.",
            AuraError(AuraErrorCode::NotFound)
        );

        diagnostics.Report(event);
    }

    ASSERT_TRUE(std::filesystem::exists(logPath));

    std::ifstream logFile(logPath);
    ASSERT_TRUE(logFile.is_open());

    std::stringstream buffer;
    buffer << logFile.rdbuf();

    const std::string contents = buffer.str();

    EXPECT_NE(contents.find("ERROR"), std::string::npos);
    EXPECT_NE(contents.find("aura.test"), std::string::npos);
    EXPECT_NE(contents.find("test.routing.event"), std::string::npos);
    EXPECT_NE(contents.find("Diagnostic routing test."), std::string::npos);
    EXPECT_NE(contents.find("NotFound"), std::string::npos);

    logFile.close();

    std::filesystem::remove(logPath);
}