#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../../Mouse.h"
#include "../Colors/AuraColors.h"

class PanelButton;
class PanelImage;
class PanelText;
class TextureCache;

// =========================================================================================
// PANEL ANCHOR
// =========================================================================================
enum class PanelAnchor
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    TopCenter,
    BottomCenter
};

// =========================================================================================
// PANEL WINDOW STATE
// =========================================================================================
enum class PanelWindowState
{
    Normal,
    Minimized,
    Maximized,
    Hidden
};

// =========================================================================================
// PANEL SCALE MODE
// =========================================================================================
enum class PanelScaleMode
{
    Fixed,
    Uniform
};

// =========================================================================================
// PANEL WINDOW OPTIONS
// =========================================================================================
struct PanelWindowOptions
{
    bool canMinimize = true;
    bool canMaximize = false;
    bool canClose = true;

    bool blocksMouseInput = true;
    bool blockWhileHovering = true;

    bool allowIndependentSizeControls = true;
};

// =========================================================================================
// PANEL RECTANGLE
// =========================================================================================
struct PanelRectangle
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    bool Contains(float pointX, float pointY) const noexcept
    {
        return
            pointX >= x &&
            pointX < x + width &&
            pointY >= y &&
            pointY < y + height;
    }
};

// =========================================================================================
// PANEL TITLE STYLE
// =========================================================================================
struct PanelTitleStyle
{
    PanelRectangle rectangle;

    DirectX::XMFLOAT4 textColor = AuraColors::Cyan;

    float textScale = 0.80f;

    DirectX::XMFLOAT2 textOffset =
    {
        0.0f,
        0.0f
    };

    bool centerHorizontally = false;
    bool centerVertically = true;
    bool visible = true;
};

// =========================================================================================
// PANEL WINDOW PLACEMENT
// =========================================================================================
struct PanelWindowPlacement
{
    DirectX::XMFLOAT2 anchorOffset =
    {
        0.0f,
        0.0f
    };
};

// =========================================================================================
// PANEL WINDOW STYLE
// =========================================================================================
struct PanelWindowStyle
{
    PanelScaleMode scaleMode = PanelScaleMode::Uniform;

    float referenceScreenWidth = 1280.0f;
    float referenceScreenHeight = 720.0f;

    float minimumScale = 0.65f;
    float maximumScale = 2.50f;

    PanelWindowPlacement placement;
    PanelTitleStyle title;

    float titleBarHeight = 48.0f;

    float buttonSize = 18.0f;
    float buttonSpacing = 4.0f;
    float buttonRightInset = 12.0f;
    float normalButtonTopInset = 26.0f;
    float minimizedButtonTopInset = 6.0f;

    DirectX::XMFLOAT4 windowButtonBackgroundColor = AuraColors::CyanButton;
    DirectX::XMFLOAT4 windowButtonTextColor = AuraColors::White;
    DirectX::XMFLOAT4 closeButtonBackgroundColor = AuraColors::RedDark;
    DirectX::XMFLOAT4 closeButtonTextColor = AuraColors::White;

    float normalHitTestTopInset = 24.0f;
    float minimizedHitTestTopInset = 0.0f;
};

// =========================================================================================
// PANEL BUTTON STYLE
// =========================================================================================
struct PanelButtonStyle
{
    DirectX::XMFLOAT4 backgroundColor =
    {
        0.10f,
        0.25f,
        0.30f,
        1.00f
    };

    DirectX::XMFLOAT4 textColor =
    {
        1.00f,
        1.00f,
        1.00f,
        1.00f
    };

    float textScale = 1.00f;
};

// =========================================================================================
// PANEL IMAGE MODE
// =========================================================================================
enum class PanelImageMode
{
    Native,
    Fit,
    Fill,
    Stretch
};

// =========================================================================================
// PANEL IMAGE STYLE
// =========================================================================================
struct PanelImageStyle
{
    PanelImageMode mode = PanelImageMode::Stretch;
};

// =========================================================================================
// PANEL WINDOW
// =========================================================================================
class PanelWindow final
{
public:
    PanelWindow(
        std::wstring title,
        PanelAnchor anchor,
        float normalWidth,
        float normalHeight,
        float minimizedHeight,
        float maximizedWidth,
        float maximizedHeight,
        PanelWindowOptions options,
        PanelWindowStyle style = {}
    );

    ~PanelWindow();

    PanelWindow(const PanelWindow&) = delete;
    PanelWindow& operator=(const PanelWindow&) = delete;
    PanelWindow(PanelWindow&&) = delete;
    PanelWindow& operator=(PanelWindow&&) = delete;

    void UpdateLayout(unsigned int screenWidth, unsigned int screenHeight) noexcept;

    void Draw(
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont,
        ID3D11ShaderResourceView* whiteTexture,
        TextureCache& textureCache
    ) const;

    bool HandleMouseEvent(const Mouse::Event& event) noexcept;
    bool ContainsPoint(float mouseX, float mouseY) const noexcept;
    bool WantsMouseCapture(const Mouse& mouse) const noexcept;

    void AddButton(
        const PanelRectangle& rectangle,
        std::wstring text,
        PanelButtonStyle style,
        std::function<void()> action
    );

    void AddImage(
        std::wstring assetPath,
        const PanelRectangle& rectangle,
        PanelImageStyle style = {}
    );

    void Minimize() noexcept;
    void Maximize() noexcept;
    void Restore() noexcept;
    void Hide() noexcept;
    void Show() noexcept;

    void ApplyResolvedLayout(
        const PanelRectangle& rectangle,
        bool isVisible
    ) noexcept;

    void RestorePreferredLayout() noexcept;

    float GetScaleX() const noexcept
    {
        return activeScaleX;
    }

    float GetScaleY() const noexcept
    {
        return activeScaleY;
    }

    float GetUniformScale() const noexcept
    {
        return (std::min)(activeScaleX, activeScaleY);
    }

    PanelRectangle LocalToScreenRectangle(
        const PanelRectangle& localRectangle
    ) const noexcept;

    const std::wstring& GetTitle() const noexcept
    {
        return title;
    }

    PanelWindowState GetState() const noexcept
    {
        return state;
    }

    bool IsVisible() const noexcept
    {
        return
            state != PanelWindowState::Hidden &&
            layoutVisible &&
            fitsCurrentViewport;
    }

    bool IsUserVisible() const noexcept
    {
        return state != PanelWindowState::Hidden;
    }

    bool IsLayoutVisible() const noexcept
    {
        return layoutVisible;
    }

    bool FitsCurrentViewport() const noexcept
    {
        return fitsCurrentViewport;
    }

    bool IsMinimized() const noexcept
    {
        return state == PanelWindowState::Minimized;
    }

    const PanelRectangle& GetPreferredRectangle() const noexcept
    {
        return preferredRectangle;
    }

    const PanelRectangle& GetWindowRectangle() const noexcept
    {
        return windowRectangle;
    }

    PanelRectangle GetTitleRectangle() const noexcept;
    PanelRectangle GetContentRectangle() const noexcept;

    PanelRectangle GetMinimizeButtonRectangle() const noexcept;
    PanelRectangle GetMaximizeButtonRectangle() const noexcept;
    PanelRectangle GetRestoreButtonRectangle() const noexcept;
    PanelRectangle GetCloseButtonRectangle() const noexcept;

    bool CanMinimize() const noexcept
    {
        return options.canMinimize;
    }

    bool CanMaximize() const noexcept
    {
        return options.canMaximize;
    }

    bool CanClose() const noexcept
    {
        return options.canClose;
    }

private:
    static float SanitizeFiniteValue(float value, float fallback) noexcept;
    static float SanitizePositiveValue(float value, float fallback) noexcept;

    static DirectX::XMFLOAT4 SanitizeColor(
        const DirectX::XMFLOAT4& color
    ) noexcept;

    static PanelRectangle SanitizeRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

    static PanelWindowStyle SanitizeStyle(
        const PanelWindowStyle& style
    ) noexcept;

    DirectX::XMFLOAT2 CalculateScales() const noexcept;
    DirectX::XMFLOAT2 GetFullDesignSize() const noexcept;

    float ScaleUniformValue(float designValue) const noexcept;

    PanelRectangle ScaleLocalRectangle(
        const PanelRectangle& localRectangle
    ) const noexcept;

    PanelRectangle CalculatePreferredRectangle(
        float activeWidth,
        float activeHeight,
        float activeMargin
    ) const noexcept;

    PanelRectangle ClampRectangleToViewport(
        const PanelRectangle& rectangle,
        float activeMargin
    ) const noexcept;

    void RecalculateRectangle() noexcept;
    void RecalculateChildRectangles() noexcept;

    float GetActiveTitleBarHeight() const noexcept;
    float GetActiveButtonTopInset() const noexcept;
    float GetActiveHitTestTopInset() const noexcept;

    PanelRectangle GetButtonRectangle(unsigned int slotFromRight) const noexcept;

    void DrawPanelImages(
        DirectX::SpriteBatch& spriteBatch,
        TextureCache& textureCache
    ) const;

    void DrawBuiltInButtons(
        DirectX::SpriteBatch& spriteBatch,
        DirectX::SpriteFont& spriteFont,
        ID3D11ShaderResourceView* whiteTexture
    ) const;

    void DrawCustomButtons(
        DirectX::SpriteBatch& spriteBatch,
        DirectX::SpriteFont& spriteFont,
        ID3D11ShaderResourceView* whiteTexture
    ) const;

private:
    std::wstring title;

    PanelAnchor anchor;
    PanelWindowOptions options;
    PanelWindowStyle style;

    PanelWindowState state = PanelWindowState::Normal;

    float normalWidth = 0.0f;
    float normalHeight = 0.0f;
    float minimizedHeight = 0.0f;
    float maximizedWidth = 0.0f;
    float maximizedHeight = 0.0f;

    unsigned int screenWidth = 0u;
    unsigned int screenHeight = 0u;

    PanelRectangle preferredRectangle;
    PanelRectangle windowRectangle;

    float activeScaleX = 1.0f;
    float activeScaleY = 1.0f;

    bool mouseInteractionStartedInside = false;
    bool layoutVisible = true;
    bool fitsCurrentViewport = false;

    std::unique_ptr<PanelText> titleText;

    std::unique_ptr<PanelButton> minimizeButton;
    std::unique_ptr<PanelButton> maximizeButton;
    std::unique_ptr<PanelButton> restoreButton;
    std::unique_ptr<PanelButton> closeButton;

    std::vector<std::unique_ptr<PanelButton>> customButtons;
    std::vector<std::unique_ptr<PanelImage>> panelImages;

    static constexpr float ScreenMargin = 18.0f;
    static constexpr float ContentPadding = 6.0f;
};