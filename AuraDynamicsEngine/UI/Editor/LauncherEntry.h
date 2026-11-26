#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "../Colors/AuraColors.h"
#include "../Window/PanelText.h"
#include "../Window/PanelWindow.h"

// =========================================================================================
// LAUNCHER ACTION TYPE
// =========================================================================================
enum class LauncherActionType
{
    None,
    OpenEditorPage
};

// =========================================================================================
// LAUNCHER ENTRY LAYOUT
// =========================================================================================
struct LauncherEntryLayout
{
    PanelRectangle rectangle =
    {
        0.0f,
        0.0f,
        120.0f,
        32.0f
    };
};

// =========================================================================================
// LAUNCHER ENTRY STYLE
// =========================================================================================
struct LauncherEntryStyle
{
    DirectX::XMFLOAT4 textColor = AuraColors::BlueSoft;
    float textScale = 0.8f;

    DirectX::XMFLOAT2 textOffset =
    {
        6.0f,
        0.0f
    };

    PanelTextHorizontalAlignment horizontalAlignment =
        PanelTextHorizontalAlignment::Left;

    PanelTextVerticalAlignment verticalAlignment =
        PanelTextVerticalAlignment::Center;

    bool fitToWidth = true;
    bool fitToHeight = true;
    bool visible = true;
};

// =========================================================================================
// LAUNCHER ENTRY ACTION
// =========================================================================================
struct LauncherEntryAction
{
    LauncherActionType type = LauncherActionType::None;
    std::wstring targetId;
};

// =========================================================================================
// EDITOR LAUNCHER ENTRY
// =========================================================================================
struct EditorLauncherEntry
{
    std::wstring id;
    std::wstring name;

    LauncherEntryLayout layout;
    LauncherEntryStyle style;

    std::vector<LauncherEntryAction> actions;
};