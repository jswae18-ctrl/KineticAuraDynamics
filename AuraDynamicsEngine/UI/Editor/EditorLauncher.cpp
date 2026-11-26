#include "EditorLauncher.h"
#include <algorithm>
#include <utility>
#include "../HUD.h"
#include "../Textures/TextureCache.h"

namespace
{
    // =====================================================================================
    // EDITOR LAUNCHER STYLE
    // =====================================================================================
    PanelWindowStyle CreateLauncherPanelStyle()
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
            100.0f
        };

        style.title.rectangle =
        {
            10.0f,
            17.0f,
            300.0f,
            50.0f
        };

        style.title.textColor = AuraColors::BlueSoft;
        style.title.textScale = 1.0f;

        style.title.textOffset =
        {
            90.0f,
            30.0f
        };

        style.title.centerHorizontally = true;
        style.title.centerVertically = true;
        style.title.visible = true;

        style.buttonSize = 16.0f;
        style.buttonSpacing = 3.0f;
        style.buttonRightInset = 24.0f;
        style.normalButtonTopInset = 34.0f;
        style.minimizedButtonTopInset = 6.0f;

        style.windowButtonBackgroundColor = AuraColors::CyanButton;
        style.windowButtonTextColor = AuraColors::White;
        style.closeButtonBackgroundColor = AuraColors::RedDark;
        style.closeButtonTextColor = AuraColors::White;

        return style;
    }

    // =====================================================================================
    // EDITOR LAUNCHER BACKGROUND
    // =====================================================================================
    HudImageConfig CreateLauncherBackgroundConfig()
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

    // =====================================================================================
    // CREATE ENTRY
    // =====================================================================================
    EditorLauncherEntry CreatePanelWindowEntry()
    {
        EditorLauncherEntry entry;

        entry.id = L"create-entry";
        entry.name = L"+ CREATE";

        entry.layout.rectangle =
        {
            30.0f,
            110.0f,
            140.0f,
            36.0f
        };

        entry.style.textColor = AuraColors::Cyan;
        entry.style.textScale = 0.9f;

        entry.style.textOffset =
        {
            8.0f,
            0.0f
        };

        entry.style.horizontalAlignment =
            PanelTextHorizontalAlignment::Left;

        entry.style.verticalAlignment =
            PanelTextVerticalAlignment::Center;

        LauncherEntryAction action;
        action.type = LauncherActionType::OpenEditorPage;
        action.targetId = L"panel-window-editor";

        entry.actions.push_back(
            std::move(action)
        );

        return entry;
    }

    // =====================================================================================
    // ENTRY TEXT
    // =====================================================================================
    std::unique_ptr<PanelText> CreateLauncherEntryText(
        const EditorLauncherEntry& entry
    )
    {
        PanelTextStyle textStyle;

        textStyle.textColor = entry.style.textColor;
        textStyle.requestedScale = entry.style.textScale;
        textStyle.horizontalAlignment = entry.style.horizontalAlignment;
        textStyle.verticalAlignment = entry.style.verticalAlignment;
        textStyle.textOffset = entry.style.textOffset;
        textStyle.fitToWidth = entry.style.fitToWidth;
        textStyle.fitToHeight = entry.style.fitToHeight;
        textStyle.visible = entry.style.visible;

        auto entryText =
            std::make_unique<PanelText>(
                entry.layout.rectangle,
                textStyle
            );

        entryText->SetText(entry.name);

        return entryText;
    }
}

// =========================================================================================
// EDITOR LAUNCHER CONSTRUCTION
// =========================================================================================
EditorLauncher::EditorLauncher(HudPanelContext&)
    :
    window(
        L"AURA EDITOR",
        PanelAnchor::TopCenter,
        500.0f,
        450.0f,
        0.0f,
        0.0f,
        0.0f,
        PanelWindowOptions
        {
            false, // canMinimize
            false, // canMaximize
            true,  // canClose
            false, // blocksMouseInput
            false,
            true   // allowIndependentSizeControls
        },
        CreateLauncherPanelStyle()
    ),
    imageConfigs
    {
        CreateLauncherBackgroundConfig()
    }
{
    window.Hide();

    launcherEntries.push_back(
        CreatePanelWindowEntry()
    );

    launcherEntryTexts.reserve(
        launcherEntries.size()
    );

    for (const EditorLauncherEntry& entry : launcherEntries)
    {
        launcherEntryTexts.push_back(
            CreateLauncherEntryText(entry)
        );
    }
}

// =========================================================================================
// EDITOR LAUNCHER UPDATE
// =========================================================================================
void EditorLauncher::Update(float) noexcept
{
}

void EditorLauncher::UpdateLayout(
    unsigned int screenWidth,
    unsigned int screenHeight
)
{
    window.UpdateLayout(
        screenWidth,
        screenHeight
    );

    const std::size_t entryCount =
        (std::min)(
            launcherEntries.size(),
            launcherEntryTexts.size()
        );

    for (std::size_t index = 0u; index < entryCount; ++index)
    {
        if (!launcherEntryTexts[index])
        {
            continue;
        }

        const PanelRectangle screenRectangle =
            window.LocalToScreenRectangle(
                launcherEntries[index].layout.rectangle
            );

        launcherEntryTexts[index]->SetScreenRectangle(
            screenRectangle
        );
    }
}

// =========================================================================================
// EDITOR LAUNCHER INPUT
// =========================================================================================
bool EditorLauncher::HandleMouseEvent(
    const Mouse::Event& event
) noexcept
{
    if (!window.IsVisible())
    {
        return false;
    }

    const PanelWindowState previousState =
        window.GetState();

    const bool windowHandledEvent =
        window.HandleMouseEvent(event);

    if (previousState != PanelWindowState::Hidden &&
        window.GetState() == PanelWindowState::Hidden)
    {
        closeRequested = true;
        return true;
    }

    if (event.GetType() != Mouse::Event::Type::LPress ||
        !window.IsVisible() ||
        window.IsMinimized())
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

    for (const EditorLauncherEntry& entry : launcherEntries)
    {
        const PanelRectangle screenRectangle =
            window.LocalToScreenRectangle(
                entry.layout.rectangle
            );

        if (!screenRectangle.Contains(mouseX, mouseY))
        {
            continue;
        }

        for (const LauncherEntryAction& action : entry.actions)
        {
            try
            {
                LauncherActionResult result =
                    actionExecutor.Execute(action);

                if (!result.openEditorPageRequested)
                {
                    continue;
                }

                requestedEditorPageId =
                    std::move(
                        result.requestedEditorPageId
                    );

                editorPageRequested = true;
            }
            catch (...)
            {
                requestedEditorPageId.clear();
                editorPageRequested = false;

                return true;
            }
        }

        return true;
    }

    return windowHandledEvent;
}

bool EditorLauncher::WantsMouseCapture(
    const Mouse& mouse
) const noexcept
{
    return window.WantsMouseCapture(mouse);
}

// =========================================================================================
// EDITOR LAUNCHER REQUESTS
// =========================================================================================
bool EditorLauncher::ConsumeCloseRequest() noexcept
{
    if (!closeRequested)
    {
        return false;
    }

    closeRequested = false;

    return true;
}

bool EditorLauncher::ConsumeEditorPageRequest(
    std::wstring& pageId
)
{
    if (!editorPageRequested)
    {
        return false;
    }

    pageId = std::move(requestedEditorPageId);

    requestedEditorPageId.clear();
    editorPageRequested = false;

    return true;
}

// =========================================================================================
// EDITOR LAUNCHER VISIBILITY
// =========================================================================================
bool EditorLauncher::IsVisible() const noexcept
{
    return window.IsVisible();
}

bool EditorLauncher::Show() noexcept
{
    const bool wasHidden =
        !window.IsVisible();

    window.Show();

    return wasHidden;
}

// =========================================================================================
// EDITOR LAUNCHER GEOMETRY
// =========================================================================================
const PanelRectangle& EditorLauncher::GetPanelWinRectangle() const noexcept
{
    return window.GetWindowRectangle();
}

PanelRectangle EditorLauncher::GetImageRectState(
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
// EDITOR LAUNCHER DRAWING
// =========================================================================================
void EditorLauncher::Draw(
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

    if (window.IsMinimized())
    {
        return;
    }

    for (const std::unique_ptr<PanelText>& entryText : launcherEntryTexts)
    {
        if (entryText)
        {
            entryText->Draw(
                spriteBatch,
                spriteFont
            );
        }
    }
}

// =========================================================================================
// AUTOMATIC HUD REGISTRATION
// =========================================================================================
REGISTER_HUD_PANEL(EditorLauncher);