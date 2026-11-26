#pragma once
#include <cstddef>
#include <string>
#include <string_view>

#include <DirectXMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "PanelWindow.h"
#include "../Colors/AuraColors.h"

// =========================================================================================
// PANEL TEXT HORIZONTAL ALIGNMENT
// =========================================================================================
enum class PanelTextHorizontalAlignment
{
    Left,
    Center,
    Right
};

// =========================================================================================
// PANEL TEXT VERTICAL ALIGNMENT
// =========================================================================================
enum class PanelTextVerticalAlignment
{
    Top,
    Center,
    Bottom
};

// =========================================================================================
// PANEL TEXT STYLE
// =========================================================================================
struct PanelTextStyle
{
    DirectX::XMFLOAT4 textColor =
        AuraColors::White;

    float requestedScale = 1.0f;
    float minimumConfiguredScale = 0.05f;
    float maximumScale = 4.0f;

    DirectX::XMFLOAT2 textOffset =
    {
        0.0f,
        0.0f
    };

    PanelTextHorizontalAlignment horizontalAlignment =
        PanelTextHorizontalAlignment::Left;

    PanelTextVerticalAlignment verticalAlignment =
        PanelTextVerticalAlignment::Center;

    bool fitToWidth = true;
    bool fitToHeight = true;
    bool allowAutomaticUpscaling = false;

    bool allowMultiline = false;

    std::size_t spacesPerTab = 4u;
    std::size_t maximumCharacters = 4096u;

    bool showTruncationMarker = true;
    bool snapToPixels = true;
    bool visible = true;
};

// =========================================================================================
// PANEL TEXT
// =========================================================================================
class PanelText final
{
public:
    PanelText(
        const PanelRectangle& localRectangle,
        PanelTextStyle style = {}
    ) noexcept;

    ~PanelText() = default;

    PanelText(const PanelText&) = delete;
    PanelText& operator=(const PanelText&) = delete;
    PanelText(PanelText&&) = delete;
    PanelText& operator=(PanelText&&) = delete;

    void SetText(
        std::wstring_view newText
    );

    const std::wstring& GetText() const noexcept
    {
        return text;
    }

    void ClearText() noexcept;

    bool IsEmpty() const noexcept
    {
        return text.empty();
    }

    bool Draw(
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont
    ) const noexcept;

    bool DrawText(
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont,
        std::wstring_view temporaryText
    ) const noexcept;

    const PanelRectangle& GetLocalRectangle() const noexcept
    {
        return localRectangle;
    }

    const PanelRectangle& GetScreenRectangle() const noexcept
    {
        return screenRectangle;
    }

    void SetLocalRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

    void SetScreenRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

    const PanelTextStyle& GetStyle() const noexcept
    {
        return style;
    }

    void SetStyle(
        const PanelTextStyle& newStyle
    );

    void SetTextColor(
        const DirectX::XMFLOAT4& color
    ) noexcept;

    void SetRequestedScale(
        float scale
    ) noexcept;

    void SetTextOffset(
        const DirectX::XMFLOAT2& offset
    ) noexcept;

    void SetHorizontalAlignment(
        PanelTextHorizontalAlignment alignment
    ) noexcept;

    void SetVerticalAlignment(
        PanelTextVerticalAlignment alignment
    ) noexcept;

    void SetVisible(
        bool isVisible
    ) noexcept;

    bool IsVisible() const noexcept
    {
        return style.visible;
    }

private:
    static constexpr std::size_t AbsoluteMaximumCharacters =
        65536u;

    static constexpr std::size_t MaximumSpacesPerTab =
        16u;

    static constexpr float AbsoluteMinimumScale =
        0.001f;

    static constexpr float AbsoluteMaximumScale =
        64.0f;

    static float SanitizeFiniteValue(
        float value,
        float fallback
    ) noexcept;

    static float ClampFloat(
        float value,
        float minimum,
        float maximum
    ) noexcept;

    static std::size_t GetSafeMaximumCharacters(
        std::size_t requestedMaximum
    ) noexcept;

    static PanelRectangle SanitizeRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

    static DirectX::XMFLOAT4 SanitizeColor(
        const DirectX::XMFLOAT4& color
    ) noexcept;

    static DirectX::XMFLOAT2 SanitizeOffset(
        const DirectX::XMFLOAT2& offset
    ) noexcept;

    static PanelTextStyle SanitizeStyle(
        const PanelTextStyle& sourceStyle
    ) noexcept;

    std::wstring SanitizeText(
        std::wstring_view sourceText
    ) const;

    static void ReplaceUnsupportedCharacters(
        std::wstring& sourceText,
        DirectX::SpriteFont& spriteFont
    );

    bool PrepareMeasuredText(
        DirectX::SpriteFont& spriteFont,
        std::wstring_view sourceText,
        std::wstring& preparedText,
        DirectX::XMFLOAT2& measuredSize
    ) const noexcept;

    bool DrawPreparedText(
        DirectX::SpriteBatch& spriteBatch,
        DirectX::SpriteFont& spriteFont,
        const std::wstring& preparedText,
        const DirectX::XMFLOAT2& measuredSize
    ) const noexcept;

    void InvalidateMeasurementCache() noexcept;

private:
    PanelRectangle localRectangle;
    PanelRectangle screenRectangle;

    PanelTextStyle style;

    std::wstring text;

    mutable const DirectX::SpriteFont* cachedFont = nullptr;
    mutable std::wstring cachedPreparedText;

    mutable DirectX::XMFLOAT2 cachedMeasuredSize =
    {
        0.0f,
        0.0f
    };

    mutable bool measurementDirty = true;
};
