#pragma once
#include <string>
#include <vector>

#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../../Camera/Camera.h"
#include "../HudImageConfig.h"
#include "../Window/PanelText.h"
#include "../Window/PanelWindow.h"

class HudPanelContext;
class TextureCache;
class Mouse;

// =========================================================================================
// CAMERA SPEED PANEL
// =========================================================================================
class CameraSpeedPanel final
{
public:
    explicit CameraSpeedPanel(
        HudPanelContext& context
    );

    ~CameraSpeedPanel() = default;

    CameraSpeedPanel(const CameraSpeedPanel&) = delete;
    CameraSpeedPanel& operator=(const CameraSpeedPanel&) = delete;
    CameraSpeedPanel(CameraSpeedPanel&&) = delete;
    CameraSpeedPanel& operator=(CameraSpeedPanel&&) = delete;

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
    std::wstring BuildSpeedInputText() const;

private:
    // Camera is owned by App and must outlive the panel.
    Camera& camera;

    PanelWindow window;

    std::vector<HudImageConfig> imageConfigs;

    PanelText speedInputText;

    float cursorTimer = 0.0f;
    bool isCursorVisible = true;

    static constexpr float CursorBlinkInterval = 0.5f;
};