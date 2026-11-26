#pragma once

#include <functional>
#include <string>

#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../UI/Window/PanelWindow.h"

// =========================================================================================
// PANEL BUTTON TYPE
// =========================================================================================
enum class PanelButtonType
{
    Custom,
    Minimize,
    Maximize,
    Restore,
    Close
};

// =========================================================================================
// PANEL BUTTON
// =========================================================================================
class PanelButton final
{
public:
    PanelButton(
        const PanelRectangle& rectangle,
        std::wstring text,
        PanelButtonType type,
        PanelButtonStyle style = {},
        std::function<void()> action = {}
    );

    ~PanelButton() = default;

    PanelButton(const PanelButton&) = delete;
    PanelButton& operator=(const PanelButton&) = delete;
    PanelButton(PanelButton&&) = delete;
    PanelButton& operator=(PanelButton&&) = delete;

    bool ContainsPoint(float mouseX, float mouseY) const noexcept;

    void Draw(
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont,
        ID3D11ShaderResourceView* whiteTexture
    ) const;

    void Invoke() const;

    const PanelRectangle& GetPanelRectangle() const noexcept
    {
        return buttonRectangle;
    }

    const PanelRectangle& GetLocalRectangle() const noexcept
    {
        return localRectangle;
    }

    const std::wstring& GetText() const noexcept
    {
        return buttonText;
    }

    PanelButtonType GetType() const noexcept
    {
        return buttonType;
    }

    void SetRectangle(const PanelRectangle& rectangle) noexcept;

private:
    static float SanitizeFiniteValue(float value, float fallback) noexcept;

    static DirectX::XMFLOAT4 SanitizeColor(
        const DirectX::XMFLOAT4& color
    ) noexcept;

    static PanelRectangle SanitizeRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

    static PanelButtonStyle SanitizeStyle(
        const PanelButtonStyle& style
    ) noexcept;

private:
    PanelButtonStyle buttonStyle;

    PanelRectangle localRectangle;
    PanelRectangle buttonRectangle;

    std::wstring buttonText;

    PanelButtonType buttonType = PanelButtonType::Custom;

    std::function<void()> action;
};