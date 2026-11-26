#include "LiveMetrics.h"
#include "../../Colors/AuraColors.h"
#include "../../HUD.h"

namespace
{
    // =====================================================================================
    // LIVE METRICS PANEL OPTIONS
    // =====================================================================================
    PanelWindowOptions CreateLiveMetricsPanelOptions() noexcept
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
    // LIVE METRICS PANEL STYLE
    // =====================================================================================
    PanelWindowStyle CreateLiveMetricsPanelStyle() noexcept
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
            -220.0f
        };

        style.title.rectangle =
        {
            0.0f,
            35.0f,
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

        style.titleBarHeight = 64.0f;

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
    // LIVE METRICS BACKGROUND IMAGE
    // =====================================================================================
    HudImageConfig CreateLiveMetricsBackgroundConfig()
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
// LIVE METRICS PANEL CONSTRUCTION
// =========================================================================================
LiveMetrics::LiveMetrics(HudPanelContext& context)
    :
    window(
        L"CPU DATA",
        PanelAnchor::BottomRight,
        300.0f,
        270.0f,
        30.0f,
        300.0f,
        270.0f,
        CreateLiveMetricsPanelOptions(),
        CreateLiveMetricsPanelStyle()
    ),
    imageConfigs
    {
        CreateLiveMetricsBackgroundConfig()
    }
{
    static_cast<void>(context);
}

// =========================================================================================
// LIVE METRICS PANEL UPDATE
// =========================================================================================
void LiveMetrics::Update(float deltaTime) noexcept
{
    static_cast<void>(deltaTime);
}

// =========================================================================================
// LIVE METRICS PANEL LAYOUT
// =========================================================================================
void LiveMetrics::UpdateLayout(unsigned int screenWidth, unsigned int screenHeight) noexcept
{
    window.UpdateLayout(screenWidth, screenHeight);
}

// =========================================================================================
// LIVE METRICS PANEL INPUT
// =========================================================================================
bool LiveMetrics::HandleMouseEvent(const Mouse::Event& event) noexcept
{
    return window.HandleMouseEvent(event);
}

// =========================================================================================
// LIVE METRICS PANEL MOUSE CAPTURE
// =========================================================================================
bool LiveMetrics::WantsMouseCapture(const Mouse& mouse) const noexcept
{
    return window.WantsMouseCapture(mouse);
}

// =========================================================================================
// LIVE METRICS PANEL STATE
// =========================================================================================
bool LiveMetrics::IsVisible() const noexcept
{
    return window.IsVisible();
}

bool LiveMetrics::Show() noexcept
{
    const bool stateChanged =
        window.GetState() != PanelWindowState::Normal;

    window.Show();

    return stateChanged;
}

// =========================================================================================
// LIVE METRICS PANEL GEOMETRY
// =========================================================================================
const PanelRectangle& LiveMetrics::GetPanelWinRectangle() const noexcept
{
    return window.GetWindowRectangle();
}

// =========================================================================================
// LIVE METRICS PANEL IMAGE GEOMETRY
// =========================================================================================
PanelRectangle LiveMetrics::GetImageRectState(const HudImageConfig& config) const noexcept
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
// LIVE METRICS PANEL DRAWING
// =========================================================================================
void LiveMetrics::Draw(
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
REGISTER_HUD_PANEL(LiveMetrics);