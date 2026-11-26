#include "PanelButton.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
    LONG RoundToLong(float value) noexcept
    {
        if (!std::isfinite(value))
        {
            return 0L;
        }

        return static_cast<LONG>(
            std::lround(value)
        );
    }
}

// =========================================================================================
// PANEL BUTTON CONSTRUCTION
// =========================================================================================
PanelButton::PanelButton(
    const PanelRectangle& rectangle,
    std::wstring text,
    PanelButtonType type,
    PanelButtonStyle style,
    std::function<void()> action
)
    :
    buttonStyle(SanitizeStyle(style)),
    localRectangle(SanitizeRectangle(rectangle)),
    buttonRectangle(SanitizeRectangle(rectangle)),
    buttonText(std::move(text)),
    buttonType(type),
    action(std::move(action))
{
}

// =========================================================================================
// PANEL BUTTON VALUE SANITIZATION
// =========================================================================================
float PanelButton::SanitizeFiniteValue(float value, float fallback) noexcept
{
    return std::isfinite(value)
        ? value
        : fallback;
}

DirectX::XMFLOAT4 PanelButton::SanitizeColor(
    const DirectX::XMFLOAT4& color
) noexcept
{
    const auto sanitizeComponent =
        [](float value) noexcept
        {
            if (!std::isfinite(value))
            {
                return 1.0f;
            }

            return (std::max)(
                0.0f,
                (std::min)(value, 1.0f)
            );
        };

    return
    {
        sanitizeComponent(color.x),
        sanitizeComponent(color.y),
        sanitizeComponent(color.z),
        sanitizeComponent(color.w)
    };
}

PanelRectangle PanelButton::SanitizeRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    return
    {
        SanitizeFiniteValue(rectangle.x, 0.0f),
        SanitizeFiniteValue(rectangle.y, 0.0f),

        (std::max)(
            0.0f,
            SanitizeFiniteValue(rectangle.width, 0.0f)
        ),

        (std::max)(
            0.0f,
            SanitizeFiniteValue(rectangle.height, 0.0f)
        )
    };
}

PanelButtonStyle PanelButton::SanitizeStyle(
    const PanelButtonStyle& sourceStyle
) noexcept
{
    PanelButtonStyle sanitizedStyle = sourceStyle;

    sanitizedStyle.backgroundColor =
        SanitizeColor(sourceStyle.backgroundColor);

    sanitizedStyle.textColor =
        SanitizeColor(sourceStyle.textColor);

    sanitizedStyle.textScale =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.textScale, 1.0f)
        );

    return sanitizedStyle;
}

// =========================================================================================
// PANEL BUTTON GEOMETRY
// =========================================================================================
void PanelButton::SetRectangle(const PanelRectangle& rectangle) noexcept
{
    buttonRectangle = SanitizeRectangle(rectangle);
}

// =========================================================================================
// PANEL BUTTON HIT TESTING
// =========================================================================================
bool PanelButton::ContainsPoint(float mouseX, float mouseY) const noexcept
{
    if (!std::isfinite(mouseX) ||
        !std::isfinite(mouseY) ||
        buttonRectangle.width <= 0.0f ||
        buttonRectangle.height <= 0.0f)
    {
        return false;
    }

    return buttonRectangle.Contains(mouseX, mouseY);
}

// =========================================================================================
// PANEL BUTTON ACTION
// =========================================================================================
void PanelButton::Invoke() const
{
    if (action)
    {
        action();
    }
}

// =========================================================================================
// PANEL BUTTON DRAWING
// =========================================================================================
void PanelButton::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    ID3D11ShaderResourceView* whiteTexture
) const
{
    if (spriteBatch == nullptr ||
        spriteFont == nullptr ||
        whiteTexture == nullptr ||
        buttonRectangle.width <= 0.0f ||
        buttonRectangle.height <= 0.0f)
    {
        return;
    }

    const LONG left = RoundToLong(buttonRectangle.x);
    const LONG top = RoundToLong(buttonRectangle.y);

    const LONG right =
        RoundToLong(
            buttonRectangle.x +
            buttonRectangle.width
        );

    const LONG bottom =
        RoundToLong(
            buttonRectangle.y +
            buttonRectangle.height
        );

    if (right <= left ||
        bottom <= top)
    {
        return;
    }

    const RECT destination =
    {
        left,
        top,
        right,
        bottom
    };

    const DirectX::XMVECTOR backgroundColor =
        DirectX::XMLoadFloat4(
            &buttonStyle.backgroundColor
        );

    spriteBatch->Draw(
        whiteTexture,
        destination,
        nullptr,
        backgroundColor
    );

    if (buttonText.empty() ||
        buttonStyle.textScale <= 0.0f)
    {
        return;
    }

    const DirectX::XMVECTOR measuredText =
        spriteFont->MeasureString(
            buttonText.c_str()
        );

    const float textWidth =
        DirectX::XMVectorGetX(
            measuredText
        );

    const float textHeight =
        DirectX::XMVectorGetY(
            measuredText
        );

    if (!std::isfinite(textWidth) ||
        !std::isfinite(textHeight) ||
        textWidth <= 0.0f ||
        textHeight <= 0.0f)
    {
        return;
    }

    const float widthScale =
        buttonRectangle.width /
        textWidth;

    const float heightScale =
        buttonRectangle.height /
        textHeight;

    const float textScale =
        (std::max)(
            0.0f,
            (std::min)(
                buttonStyle.textScale,
                (std::min)(
                    widthScale,
                    heightScale
                )
            )
        );

    if (!std::isfinite(textScale) ||
        textScale <= 0.0f)
    {
        return;
    }

    const float scaledTextWidth =
        textWidth *
        textScale;

    const float scaledTextHeight =
        textHeight *
        textScale;

    const float textX =
        buttonRectangle.x +
        (
            buttonRectangle.width -
            scaledTextWidth
        ) *
        0.5f;

    const float textY =
        buttonRectangle.y +
        (
            buttonRectangle.height -
            scaledTextHeight
        ) *
        0.5f;

    if (!std::isfinite(textX) ||
        !std::isfinite(textY))
    {
        return;
    }

    const DirectX::XMVECTOR textColor =
        DirectX::XMLoadFloat4(
            &buttonStyle.textColor
        );

    spriteFont->DrawString(
        spriteBatch,
        buttonText.c_str(),
        DirectX::XMFLOAT2
        {
            textX,
            textY
        },
        textColor,
        0.0f,
        DirectX::XMFLOAT2
        {
            0.0f,
            0.0f
        },
        textScale
    );
}