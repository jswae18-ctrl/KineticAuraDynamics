#include "CameraSpeedPanel.h"
#include "../Colors/AuraColors.h"
#include "../HUD.h"
#include <cmath>
#include <string>
#include <string_view>

namespace
{
    constexpr std::wstring_view IdleInputText =
        L"-> [PRESS TAB TO ENTER SPEED]";

    constexpr std::wstring_view InputPrefix =
        L"-> [";

    constexpr std::wstring_view InputSuffix =
        L"]";

    constexpr wchar_t CursorCharacter =
        L'|';

    // =====================================================================================
    // PANEL WINDOW OPTIONS
    // =====================================================================================
    PanelWindowOptions CreateCameraSpeedPanelOptions() noexcept
    {
        PanelWindowOptions options;

        options.canMinimize =
            false;

        options.canMaximize =
            false;

        options.canClose =
            false;

        options.blocksMouseInput =
            true;

        options.blockWhileHovering =
            false;

        options.allowIndependentSizeControls =
            true;

        return options;
    }

    // =====================================================================================
    // PANEL WINDOW STYLE
    // =====================================================================================
    PanelWindowStyle CreateCameraSpeedPanelStyle() noexcept
    {
        PanelWindowStyle style;

        style.scaleMode =
            PanelScaleMode::Fixed;

        style.referenceScreenWidth =
            1280.0f;

        style.referenceScreenHeight =
            720.0f;

        style.minimumScale =
            0.65f;

        style.maximumScale =
            2.50f;

        style.placement.anchorOffset =
        {
            0.0f,
            -85.0f
        };

        style.title.rectangle =
        {
            0.0f,
            18.0f,
            300.0f,
            20.0f
        };

        style.title.textColor =
            AuraColors::BlueSoft;

        style.title.textScale =
            0.70f;

        style.title.textOffset =
        {
            0.0f,
            0.0f
        };

        style.title.centerHorizontally =
            true;

        style.title.centerVertically =
            true;

        style.title.visible =
            true;

        style.titleBarHeight =
            48.0f;

        style.buttonSize =
            16.0f;

        style.buttonSpacing =
            3.0f;

        style.buttonRightInset =
            24.0f;

        style.normalButtonTopInset =
            20.0f;

        style.minimizedButtonTopInset =
            6.0f;

        style.windowButtonBackgroundColor =
            AuraColors::BlueDark;

        style.windowButtonTextColor =
            AuraColors::Cyan;

        style.closeButtonBackgroundColor =
            AuraColors::RedDark;

        style.closeButtonTextColor =
            AuraColors::White;

        style.normalHitTestTopInset =
            24.0f;

        style.minimizedHitTestTopInset =
            0.0f;

        return style;
    }

    // =====================================================================================
    // CAMERA SPEED INPUT TEXT STYLE
    // =====================================================================================
    PanelTextStyle CreateCameraSpeedInputTextStyle() noexcept
    {
        PanelTextStyle style;

        style.textColor =
            AuraColors::Cyan;

        style.requestedScale =
            0.60f;

        style.minimumConfiguredScale =
            0.05f;

        style.maximumScale =
            1.50f;

        style.textOffset =
        {
            0.0f,
            0.0f
        };

        style.horizontalAlignment =
            PanelTextHorizontalAlignment::Left;

        style.verticalAlignment =
            PanelTextVerticalAlignment::Center;

        style.fitToWidth =
            true;

        style.fitToHeight =
            true;

        style.allowAutomaticUpscaling =
            false;

        style.allowMultiline =
            false;

        style.spacesPerTab =
            4u;

        style.maximumCharacters =
            128u;

        style.showTruncationMarker =
            true;

        style.snapToPixels =
            true;

        style.visible =
            true;

        return style;
    }

    // =====================================================================================
    // CAMERA SPEED BACKGROUND IMAGE
    // =====================================================================================
    HudImageConfig CreateCameraSpeedBackgroundConfig()
    {
        HudImageConfig config;

        config.assetPath =
            L"UI\\Panels\\CameraSpeedPanelImage\\SmallPanel.png";

        config.tintColor =
            AuraColors::BlueDark;

        config.followsWindowRectangle =
            true;

        config.visibleWhileNormal =
            true;

        config.visibleWhileMinimized =
            true;

        config.visibleWhileMaximized =
            true;

        return config;
    }

    // =====================================================================================
    // CAMERA SPEED INPUT IMAGE
    // =====================================================================================
    HudImageConfig CreateCameraSpeedInputConfig()
    {
        HudImageConfig config;

        config.assetPath =
            L"UI\\Panels\\CameraSpeedPanelImage\\SpeedInputPanel.png";

        config.tintColor =
            AuraColors::BlueSoft;

        config.localRectangle =
        {
            12.0f,
            57.0f,
            276.0f,
            100.0f
        };

        config.followsWindowRectangle =
            false;

        config.visibleWhileNormal =
            true;

        config.visibleWhileMinimized =
            false;

        config.visibleWhileMaximized =
            true;

        return config;
    }
}

// =========================================================================================
// CAMERA SPEED PANEL CONSTRUCTION
// =========================================================================================
CameraSpeedPanel::CameraSpeedPanel(
    HudPanelContext& context
)
    :
    camera(
        context.Require<Camera>()
    ),
    window(
        L"CAMERA SPEED",
        PanelAnchor::BottomRight,
        300.0f,
        160.0f,
        30.0f,
        800.0f,
        160.0f,
        CreateCameraSpeedPanelOptions(),
        CreateCameraSpeedPanelStyle()
    ),
    imageConfigs
    {
        CreateCameraSpeedBackgroundConfig(),
        CreateCameraSpeedInputConfig()
    },
    speedInputText(
        PanelRectangle
        {
            42.0f,
            82.0f,
            216.0f,
            45.0f
        },
        CreateCameraSpeedInputTextStyle()
    )
{
}

// =========================================================================================
// CAMERA SPEED PANEL UPDATE
// =========================================================================================
void CameraSpeedPanel::Update(
    float deltaTime
) noexcept
{
    if (!camera.IsEnteringSpeed())
    {
        cursorTimer =
            0.0f;

        isCursorVisible =
            true;

        return;
    }

    if (!std::isfinite(deltaTime) ||
        deltaTime <= 0.0f)
    {
        return;
    }

    cursorTimer +=
        deltaTime;

    if (!std::isfinite(cursorTimer))
    {
        cursorTimer =
            0.0f;

        isCursorVisible =
            true;

        return;
    }

    while (cursorTimer >=
        CursorBlinkInterval)
    {
        cursorTimer -=
            CursorBlinkInterval;

        isCursorVisible =
            !isCursorVisible;
    }
}

// =========================================================================================
// CAMERA SPEED PANEL LAYOUT
// =========================================================================================
void CameraSpeedPanel::UpdateLayout(
    unsigned int screenWidth,
    unsigned int screenHeight
) noexcept
{
    window.UpdateLayout(
        screenWidth,
        screenHeight
    );

    speedInputText.SetScreenRectangle(
        window.LocalToScreenRectangle(
            speedInputText.GetLocalRectangle()
        )
    );
}

// =========================================================================================
// CAMERA SPEED PANEL INPUT
// =========================================================================================
bool CameraSpeedPanel::HandleMouseEvent(
    const Mouse::Event& event
) noexcept
{
    const PanelWindowState previousState =
        window.GetState();

    const bool handled =
        window.HandleMouseEvent(
            event
        );

    const PanelWindowState currentState =
        window.GetState();

    const bool becameUnavailable =
        previousState != currentState &&
        (
            currentState == PanelWindowState::Minimized ||
            currentState == PanelWindowState::Hidden
        );

    if (camera.IsEnteringSpeed() &&
        becameUnavailable)
    {
        camera.CancelSpeedEntry();

        cursorTimer =
            0.0f;

        isCursorVisible =
            true;
    }

    return handled;
}

// =========================================================================================
// CAMERA SPEED PANEL MOUSE CAPTURE
// =========================================================================================
bool CameraSpeedPanel::WantsMouseCapture(
    const Mouse& mouse
) const noexcept
{
    return window.WantsMouseCapture(
        mouse
    );
}

// =========================================================================================
// CAMERA SPEED PANEL STATE
// =========================================================================================
bool CameraSpeedPanel::IsVisible() const noexcept
{
    return window.IsVisible();
}

bool CameraSpeedPanel::Show() noexcept
{
    const bool stateChanged =
        window.GetState() !=
        PanelWindowState::Normal;

    window.Show();

    return stateChanged;
}

// =========================================================================================
// CAMERA SPEED PANEL GEOMETRY
// =========================================================================================
const PanelRectangle&
CameraSpeedPanel::GetPanelWinRectangle() const noexcept
{
    return window.GetWindowRectangle();
}

// =========================================================================================
// CAMERA SPEED PANEL IMAGE GEOMETRY
// =========================================================================================
PanelRectangle CameraSpeedPanel::GetImageRectState(
    const HudImageConfig& config
) const noexcept
{
    if (!window.IsVisible())
    {
        return {};
    }

    switch (window.GetState())
    {
    case PanelWindowState::Normal:
        if (!config.visibleWhileNormal)
        {
            return {};
        }
        break;

    case PanelWindowState::Minimized:
        if (!config.visibleWhileMinimized)
        {
            return {};
        }
        break;

    case PanelWindowState::Maximized:
        if (!config.visibleWhileMaximized)
        {
            return {};
        }
        break;

    case PanelWindowState::Hidden:
        return {};
    }

    if (config.followsWindowRectangle)
    {
        return window.GetWindowRectangle();
    }

    return window.LocalToScreenRectangle(
        config.localRectangle
    );
}

// =========================================================================================
// CAMERA SPEED INPUT TEXT
// =========================================================================================
std::wstring CameraSpeedPanel::BuildSpeedInputText() const
{
    if (!camera.IsEnteringSpeed())
    {
        return std::wstring(
            IdleInputText
        );
    }

    const std::string& speedInput =
        camera.GetSpeedInput();

    std::wstring inputText;

    inputText.reserve(
        InputPrefix.size() +
        speedInput.size() +
        InputSuffix.size() +
        1u
    );

    inputText.append(
        InputPrefix
    );

    for (const char character : speedInput)
    {
        inputText.push_back(
            static_cast<wchar_t>(
                static_cast<unsigned char>(
                    character
                )
            )
        );
    }

    if (isCursorVisible)
    {
        inputText.push_back(
            CursorCharacter
        );
    }

    inputText.append(
        InputSuffix
    );

    return inputText;
}

// =========================================================================================
// CAMERA SPEED PANEL DRAWING
// =========================================================================================
void CameraSpeedPanel::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    ID3D11ShaderResourceView* whiteTexture,
    unsigned int screenWidth,
    unsigned int screenHeight,
    TextureCache& textureCache
)
{
    static_cast<void>(
        screenWidth
    );

    static_cast<void>(
        screenHeight
    );

    if (!window.IsVisible() ||
        spriteBatch == nullptr ||
        spriteFont == nullptr ||
        whiteTexture == nullptr)
    {
        return;
    }

    window.Draw(
        spriteBatch,
        spriteFont,
        whiteTexture,
        textureCache
    );

    if (window.IsMinimized())
    {
        return;
    }

    speedInputText.SetText(
        BuildSpeedInputText()
    );

    speedInputText.Draw(
        spriteBatch,
        spriteFont
    );
}

// =========================================================================================
// AUTOMATIC HUD PANEL REGISTRATION
// =========================================================================================
REGISTER_HUD_PANEL(CameraSpeedPanel);