#pragma once
#include <vector>
#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../../HudImageConfig.h"
#include "../../Window/PanelWindow.h"

class HudPanelContext;
class TextureCache;
class Mouse;

// =========================================================================================
// SYSTEM STATUS PANEL
// =========================================================================================
class SystemStatus final
{
public:
    explicit SystemStatus(HudPanelContext& context);

    ~SystemStatus() = default;

    SystemStatus(const SystemStatus&) = delete;
    SystemStatus& operator=(const SystemStatus&) = delete;
    SystemStatus(SystemStatus&&) = delete;
    SystemStatus& operator=(SystemStatus&&) = delete;

    void Update(float deltaTime) noexcept;

    void UpdateLayout(unsigned int screenWidth, unsigned int screenHeight) noexcept;

    bool HandleMouseEvent(const Mouse::Event& event) noexcept;

    bool WantsMouseCapture(const Mouse& mouse) const noexcept;

    bool IsVisible() const noexcept;

    bool Show() noexcept;

    const PanelRectangle& GetPanelWinRectangle() const noexcept;

    const std::vector<HudImageConfig>& GetImageConfigs() const noexcept
    {
        return imageConfigs;
    }

    PanelRectangle GetImageRectState(const HudImageConfig& config) const noexcept;

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
};