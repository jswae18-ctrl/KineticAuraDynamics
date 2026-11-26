#pragma once
#include <vector>
#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "../../Mouse.h"
#include "../HudImageConfig.h"
#include "../Window/PanelWindow.h"

class HudPanelContext;
class TextureCache;

// =========================================================================================
// AURA EDITOR
// =========================================================================================
class AuraEditor final
{
public:
    explicit AuraEditor(HudPanelContext& context);

    ~AuraEditor() = default;

    AuraEditor(const AuraEditor&) = delete;
    AuraEditor& operator=(const AuraEditor&) = delete;
    AuraEditor(AuraEditor&&) = delete;
    AuraEditor& operator=(AuraEditor&&) = delete;

    void Update(float deltaTime) noexcept;
    void UpdateLayout(unsigned int screenWidth, unsigned int screenHeight);

    bool HandleMouseEvent(const Mouse::Event& event) noexcept;
    bool WantsMouseCapture(const Mouse& mouse) const noexcept;

    bool ConsumeLauncherRequest() noexcept;

    bool IsVisible() const noexcept;
    bool Show() noexcept;
    void Hide() noexcept;

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
    std::vector<HudImageConfig> imageConfigs;

    bool launcherRequested = false;
};