#pragma once
#include <string>
#include "LauncherEntry.h"

// =========================================================================================
// LAUNCHER ACTION RESULT
// =========================================================================================
struct LauncherActionResult
{
    bool handled = false;
    bool openEditorPageRequested = false;

    std::wstring requestedEditorPageId;
};

// =========================================================================================
// LAUNCHER ACTION EXECUTOR
// =========================================================================================
class LauncherActionExecutor final
{
public:
    LauncherActionResult Execute(
        const LauncherEntryAction& action
    ) const;
};