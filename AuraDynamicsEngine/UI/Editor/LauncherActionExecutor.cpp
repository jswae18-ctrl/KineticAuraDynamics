#include "LauncherActionExecutor.h"

// =========================================================================================
// LAUNCHER ACTION EXECUTION
// =========================================================================================
LauncherActionResult LauncherActionExecutor::Execute(
    const LauncherEntryAction& action
) const
{
    LauncherActionResult result;

    switch (action.type)
    {
    case LauncherActionType::None:
        break;

    case LauncherActionType::OpenEditorPage:
        if (action.targetId.empty())
        {
            break;
        }

        result.handled = true;
        result.openEditorPageRequested = true;
        result.requestedEditorPageId = action.targetId;
        break;
    }

    return result;
}