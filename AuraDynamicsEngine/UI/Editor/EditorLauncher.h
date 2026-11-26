#pragma once
#include <memory>
#include <string>
#include <vector>
#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../../Mouse.h"
#include "../HudImageConfig.h"
#include "../Window/PanelText.h"
#include "../Window/PanelWindow.h"
#include "LauncherActionExecutor.h"
#include "LauncherEntry.h"

class HudPanelContext;
class TextureCache;

// =========================================================================================
// EDITOR LAUNCHER
// =========================================================================================
class EditorLauncher final
{
public:
    explicit EditorLauncher(HudPanelContext& context);

    ~EditorLauncher() = default;

    EditorLauncher(const EditorLauncher&) = delete;
    EditorLauncher& operator=(const EditorLauncher&) = delete;
    EditorLauncher(EditorLauncher&&) = delete;
    EditorLauncher& operator=(EditorLauncher&&) = delete;

    void Update(float deltaTime) noexcept;
    void UpdateLayout(unsigned int screenWidth, unsigned int screenHeight);

    bool HandleMouseEvent(const Mouse::Event& event) noexcept;
    bool WantsMouseCapture(const Mouse& mouse) const noexcept;

    bool ConsumeCloseRequest() noexcept;
    bool ConsumeEditorPageRequest(std::wstring& pageId);

    bool IsVisible() const noexcept;
    bool Show() noexcept;

    const PanelRectangle& GetPanelWinRectangle() const noexcept;

    const std::vector<HudImageConfig>& GetImageConfigs() const noexcept
    {
        return imageConfigs;
    }

    PanelRectangle GetImageRectState(
        const HudImageConfig& config
    ) const noexcept;

    void Draw(
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont,
        ID3D11ShaderResourceView* whiteTexture,
        unsigned int screenWidth,
        unsigned int screenHeight,
        TextureCache& textureCache
    );

private:
    PanelWindow window;

    std::vector<EditorLauncherEntry> launcherEntries;
    std::vector<std::unique_ptr<PanelText>> launcherEntryTexts;
    std::vector<HudImageConfig> imageConfigs;

    LauncherActionExecutor actionExecutor;

    bool closeRequested = false;
    bool editorPageRequested = false;

    std::wstring requestedEditorPageId;
};