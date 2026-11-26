#pragma once
#include <string>
#include <DirectXMath.h>
#include "../UI/Window/PanelWindow.h"
#include "../UI/Colors/AuraColors.h"

// =========================================================================================
// HUD IMAGE CONFIGURATION
// =========================================================================================
struct HudImageConfig
{
    std::wstring assetPath;

    DirectX::XMFLOAT4 tintColor = AuraColors::White;

    // Rectangle local to the owning panel.
    PanelRectangle localRectangle =
    {
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    bool followsWindowRectangle = false;

    bool visibleWhileNormal = true;
    bool visibleWhileMinimized = false;
    bool visibleWhileMaximized = true;
};