#include "AuraEditor.h"
#include "../HUD.h"
#include "../Textures/TextureCache.h"

namespace
{
    // =====================================================================================
    // AURA EDITOR PANEL STYLE
    // =====================================================================================
    PanelWindowStyle CreateEditorPanelStyle()
    {
        PanelWindowStyle style;

        style.scaleMode = PanelScaleMode::Uniform;

        style.referenceScreenWidth = 1280;
        style.referenceScreenHeight = 720;

        style.minimumScale = 0.65f;
        style.maximumScale = 2.5f;

        style.placement.anchorOffset =
        {
            0.0f,
            50.0f
        };

        style.title.rectangle =
        {
            10.0f,
            17.0f,
            50.0f,
            20.0f
        };

        style.title.textColor = AuraColors::BlueSoft;
        style.title.textScale = 0.80f;

        style.title.textOffset =
        {
            0.0f,
            0.0f
        };

        style.title.centerHorizontally = true;
        style.title.centerVertically = true;
        style.title.visible = true;

        style.buttonSize = 16.0f;
        style.buttonSpacing = 3.0f;
        style.buttonRightInset = 24.0f;
        style.normalButtonTopInset = 34.0f;
        style.minimizedButtonTopInset = 6.0f;

        return style;
    }

    // =====================================================================================
    // AURA EDITOR BACKGROUND
    // =====================================================================================
    HudImageConfig CreateEditorBackgroundConfig()
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
// AURA EDITOR CONSTRUCTION
// =========================================================================================
AuraEditor::AuraEditor(HudPanelContext&)
    :
    window(
        L"AURA EDITOR",
        PanelAnchor::TopLeft,
        70.0f,
        50.0f,
        0.0f,
        500.0f,
        300.0f,
        PanelWindowOptions
        {
            false,
            false,
            false,
            true,
            false,
            true
        },
        CreateEditorPanelStyle()
    ),
    imageConfigs
    {
        CreateEditorBackgroundConfig()
    }
{
    window.Show();
}

// =========================================================================================
// AURA EDITOR UPDATE
// =========================================================================================
void AuraEditor::Update(float) noexcept
{
}

void AuraEditor::UpdateLayout(
    unsigned int screenWidth,
    unsigned int screenHeight
)
{
    window.UpdateLayout(
        screenWidth,
        screenHeight
    );
}

// =========================================================================================
// AURA EDITOR INPUT
// =========================================================================================
bool AuraEditor::HandleMouseEvent(
    const Mouse::Event& event
) noexcept
{
    if (!window.IsVisible())
    {
        return false;
    }

    const bool windowHandledEvent =
        window.HandleMouseEvent(event);

    if (event.GetType() != Mouse::Event::Type::LPress)
    {
        return windowHandledEvent;
    }

    const float mouseX =
        static_cast<float>(
            event.GetPosX()
        );

    const float mouseY =
        static_cast<float>(
            event.GetPosY()
        );

    if (!window.ContainsPoint(mouseX, mouseY))
    {
        return windowHandledEvent;
    }

    launcherRequested = true;

    return true;
}

bool AuraEditor::WantsMouseCapture(
    const Mouse& mouse
) const noexcept
{
    return window.WantsMouseCapture(mouse);
}

// =========================================================================================
// AURA EDITOR REQUESTS
// =========================================================================================
bool AuraEditor::ConsumeLauncherRequest() noexcept
{
    if (!launcherRequested)
    {
        return false;
    }

    launcherRequested = false;

    return true;
}

// =========================================================================================
// AURA EDITOR VISIBILITY
// =========================================================================================
bool AuraEditor::IsVisible() const noexcept
{
    return window.IsVisible();
}

bool AuraEditor::Show() noexcept
{
    const bool wasHidden =
        !window.IsVisible();

    window.Show();

    return wasHidden;
}

void AuraEditor::Hide() noexcept
{
    window.Hide();
}

// =========================================================================================
// AURA EDITOR GEOMETRY
// =========================================================================================
const PanelRectangle& AuraEditor::GetPanelWinRectangle() const noexcept
{
    return window.GetWindowRectangle();
}

PanelRectangle AuraEditor::GetImageRectState(
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
// AURA EDITOR DRAWING
// =========================================================================================
void AuraEditor::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    ID3D11ShaderResourceView* whiteTexture,
    unsigned int,
    unsigned int,
    TextureCache& textureCache
)
{
    if (spriteBatch == nullptr ||
        spriteFont == nullptr ||
        whiteTexture == nullptr ||
        !window.IsVisible())
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
// AUTOMATIC HUD REGISTRATION
// =========================================================================================
REGISTER_HUD_PANEL(AuraEditor);