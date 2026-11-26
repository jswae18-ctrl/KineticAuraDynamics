#include "SystemStatus.h"

#include "../../Colors/AuraColors.h"
#include "../../HUD.h"

namespace
{
    // =====================================================================================
    // SYSTEM STATUS PANEL OPTIONS
    // =====================================================================================
    PanelWindowOptions CreateSystemStatusPanelOptions() noexcept
    {
        PanelWindowOptions options;

        options.canMinimize = false;
        options.canMaximize = false;
        options.canClose = false;

        options.blocksMouseInput = true;
        options.blockWhileHovering = false;

        options.allowIndependentSizeControls = true;

        return options;
    }

    // =====================================================================================
    // SYSTEM STATUS PANEL STYLE
    // =====================================================================================
    PanelWindowStyle CreateSystemStatusPanelStyle() noexcept
    {
        PanelWindowStyle style;

        style.scaleMode = PanelScaleMode::Fixed;

        style.referenceScreenWidth = 1280.0f;
        style.referenceScreenHeight = 720.0f;

        style.minimumScale = 0.65f;
        style.maximumScale = 2.50f;

        style.placement.anchorOffset =
        {
            0.0f,
            60.0f
        };

        style.title.rectangle =
        {
            0.0f,
            17.0f,
            300.0f,
            20.0f
        };

        style.title.textColor = AuraColors::BlueSoft;
        style.title.textScale = 0.90f;

        style.title.textOffset =
        {
            0.0f,
            0.0f
        };

        style.title.centerHorizontally = true;
        style.title.centerVertically = true;
        style.title.visible = true;

        style.titleBarHeight = 48.0f;

        style.buttonSize = 16.0f;
        style.buttonSpacing = 3.0f;
        style.buttonRightInset = 24.0f;

        style.normalButtonTopInset = 34.0f;
        style.minimizedButtonTopInset = 6.0f;

        style.windowButtonBackgroundColor = AuraColors::CyanButton;
        style.windowButtonTextColor = AuraColors::White;

        style.closeButtonBackgroundColor = AuraColors::RedDark;
        style.closeButtonTextColor = AuraColors::White;

        style.normalHitTestTopInset = 24.0f;
        style.minimizedHitTestTopInset = 0.0f;

        return style;
    }

    // =====================================================================================
    // SYSTEM STATUS BACKGROUND IMAGE
    // =====================================================================================
    HudImageConfig CreateSystemStatusBackgroundConfig()
    {
        HudImageConfig config;

        config.assetPath =
            L"UI\\Panels\\CameraSpeedPanelImage\\SmallPanel.png";

        config.tintColor = AuraColors::BlueDark;

        config.followsWindowRectangle = true;

        config.visibleWhileNormal = true;
        config.visibleWhileMinimized = true;
        config.visibleWhileMaximized = true;

        return config;
    }
}

// =========================================================================================
// SYSTEM STATUS PANEL CONSTRUCTION
// =========================================================================================
SystemStatus::SystemStatus(HudPanelContext& context)
    :
    window(
        L"SYSTEM STATUS",
        PanelAnchor::TopRight,
        300.0f,
        160.0f,
        30.0f,
        300.0f,
        160.0f,
        CreateSystemStatusPanelOptions(),
        CreateSystemStatusPanelStyle()
    ),
    imageConfigs
    {
        CreateSystemStatusBackgroundConfig()
    }
{
    static_cast<void>(context);
}

// =========================================================================================
// SYSTEM STATUS PANEL UPDATE
// =========================================================================================
void SystemStatus::Update(float deltaTime) noexcept
{
    static_cast<void>(deltaTime);
}

// =========================================================================================
// SYSTEM STATUS PANEL LAYOUT
// =========================================================================================
void SystemStatus::UpdateLayout(unsigned int screenWidth, unsigned int screenHeight) noexcept
{
    window.UpdateLayout(screenWidth, screenHeight);
}

// =========================================================================================
// SYSTEM STATUS PANEL INPUT
// =========================================================================================
bool SystemStatus::HandleMouseEvent(const Mouse::Event& event) noexcept
{
    return window.HandleMouseEvent(event);
}

// =========================================================================================
// SYSTEM STATUS PANEL MOUSE CAPTURE
// =========================================================================================
bool SystemStatus::WantsMouseCapture(const Mouse& mouse) const noexcept
{
    return window.WantsMouseCapture(mouse);
}

// =========================================================================================
// SYSTEM STATUS PANEL STATE
// =========================================================================================
bool SystemStatus::IsVisible() const noexcept
{
    return window.IsVisible();
}

bool SystemStatus::Show() noexcept
{
    const bool stateChanged =
        window.GetState() != PanelWindowState::Normal;

    window.Show();

    return stateChanged;
}

// =========================================================================================
// SYSTEM STATUS PANEL GEOMETRY
// =========================================================================================
const PanelRectangle& SystemStatus::GetPanelWinRectangle() const noexcept
{
    return window.GetWindowRectangle();
}

// =========================================================================================
// SYSTEM STATUS PANEL IMAGE GEOMETRY
// =========================================================================================
PanelRectangle SystemStatus::GetImageRectState(const HudImageConfig& config) const noexcept
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

    return window.LocalToScreenRectangle(config.localRectangle);
}

// =========================================================================================
// SYSTEM STATUS PANEL DRAWING
// =========================================================================================
void SystemStatus::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    ID3D11ShaderResourceView* whiteTexture,
    unsigned int screenWidth,
    unsigned int screenHeight,
    TextureCache& textureCache
)
{
    static_cast<void>(screenWidth);
    static_cast<void>(screenHeight);

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
}

// =========================================================================================
// AUTOMATIC HUD PANEL REGISTRATION
// =========================================================================================
REGISTER_HUD_PANEL(SystemStatus);