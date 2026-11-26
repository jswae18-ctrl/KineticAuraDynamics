#include "PanelText.h"
#include <algorithm>
#include <cmath>
#include <utility>

// =========================================================================================
// PANEL TEXT CONSTRUCTION
// =========================================================================================
PanelText::PanelText(
    const PanelRectangle& localRectangle,
    PanelTextStyle style
) noexcept
    :
    localRectangle(
        SanitizeRectangle(
            localRectangle
        )
    ),
    screenRectangle(
        SanitizeRectangle(
            localRectangle
        )
    ),
    style(
        SanitizeStyle(
            style
        )
    )
{
}

// =========================================================================================
// PANEL TEXT CONTENT
// =========================================================================================
void PanelText::SetText(
    std::wstring_view newText
)
{
    std::wstring sanitizedText =
        SanitizeText(
            newText
        );

    if (text ==
        sanitizedText)
    {
        return;
    }

    text =
        std::move(
            sanitizedText
        );

    InvalidateMeasurementCache();
}

void PanelText::ClearText() noexcept
{
    if (text.empty())
    {
        return;
    }

    text.clear();

    InvalidateMeasurementCache();
}

// =========================================================================================
// PANEL TEXT DRAWING
// =========================================================================================
bool PanelText::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont
) const noexcept
{
    if (spriteBatch == nullptr ||
        spriteFont == nullptr ||
        !style.visible ||
        text.empty() ||
        screenRectangle.width <= 0.0f ||
        screenRectangle.height <= 0.0f)
    {
        return false;
    }

    if (measurementDirty ||
        cachedFont != spriteFont)
    {
        std::wstring preparedText;

        DirectX::XMFLOAT2 measuredSize =
        {
            0.0f,
            0.0f
        };

        if (!PrepareMeasuredText(
            *spriteFont,
            text,
            preparedText,
            measuredSize
        ))
        {
            return false;
        }

        cachedPreparedText =
            std::move(
                preparedText
            );

        cachedMeasuredSize =
            measuredSize;

        cachedFont =
            spriteFont;

        measurementDirty =
            false;
    }

    return DrawPreparedText(
        *spriteBatch,
        *spriteFont,
        cachedPreparedText,
        cachedMeasuredSize
    );
}

bool PanelText::DrawText(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    std::wstring_view temporaryText
) const noexcept
{
    if (spriteBatch == nullptr ||
        spriteFont == nullptr ||
        !style.visible ||
        temporaryText.empty() ||
        screenRectangle.width <= 0.0f ||
        screenRectangle.height <= 0.0f)
    {
        return false;
    }

    std::wstring preparedText;

    DirectX::XMFLOAT2 measuredSize =
    {
        0.0f,
        0.0f
    };

    if (!PrepareMeasuredText(
        *spriteFont,
        temporaryText,
        preparedText,
        measuredSize
    ))
    {
        return false;
    }

    return DrawPreparedText(
        *spriteBatch,
        *spriteFont,
        preparedText,
        measuredSize
    );
}

// =========================================================================================
// PANEL TEXT RECTANGLES
// =========================================================================================
void PanelText::SetLocalRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    localRectangle =
        SanitizeRectangle(
            rectangle
        );
}

void PanelText::SetScreenRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    screenRectangle =
        SanitizeRectangle(
            rectangle
        );
}

// =========================================================================================
// PANEL TEXT STYLE
// =========================================================================================
void PanelText::SetStyle(
    const PanelTextStyle& newStyle
)
{
    style =
        SanitizeStyle(
            newStyle
        );

    text =
        SanitizeText(
            text
        );

    InvalidateMeasurementCache();
}

void PanelText::SetTextColor(
    const DirectX::XMFLOAT4& color
) noexcept
{
    style.textColor =
        SanitizeColor(
            color
        );
}

void PanelText::SetRequestedScale(
    float scale
) noexcept
{
    const float sanitizedScale =
        SanitizeFiniteValue(
            scale,
            style.requestedScale
        );

    style.requestedScale =
        ClampFloat(
            sanitizedScale,
            style.minimumConfiguredScale,
            style.maximumScale
        );
}

void PanelText::SetTextOffset(
    const DirectX::XMFLOAT2& offset
) noexcept
{
    style.textOffset =
        SanitizeOffset(
            offset
        );
}

void PanelText::SetHorizontalAlignment(
    PanelTextHorizontalAlignment alignment
) noexcept
{
    style.horizontalAlignment =
        alignment;
}

void PanelText::SetVerticalAlignment(
    PanelTextVerticalAlignment alignment
) noexcept
{
    style.verticalAlignment =
        alignment;
}

void PanelText::SetVisible(
    bool isVisible
) noexcept
{
    style.visible =
        isVisible;
}

// =========================================================================================
// PANEL TEXT VALUE SANITIZATION
// =========================================================================================
float PanelText::SanitizeFiniteValue(
    float value,
    float fallback
) noexcept
{
    return std::isfinite(value)
        ? value
        : fallback;
}

float PanelText::ClampFloat(
    float value,
    float minimum,
    float maximum
) noexcept
{
    if (minimum >
        maximum)
    {
        const float previousMinimum =
            minimum;

        minimum =
            maximum;

        maximum =
            previousMinimum;
    }

    return
        (std::max)(
            minimum,
            (std::min)(
                value,
                maximum
            )
        );
}

std::size_t PanelText::GetSafeMaximumCharacters(
    std::size_t requestedMaximum
) noexcept
{
    return
        (std::min)(
            requestedMaximum,
            AbsoluteMaximumCharacters
        );
}

PanelRectangle PanelText::SanitizeRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    return
    {
        SanitizeFiniteValue(
            rectangle.x,
            0.0f
        ),

        SanitizeFiniteValue(
            rectangle.y,
            0.0f
        ),

        (std::max)(
            0.0f,
            SanitizeFiniteValue(
                rectangle.width,
                0.0f
            )
        ),

        (std::max)(
            0.0f,
            SanitizeFiniteValue(
                rectangle.height,
                0.0f
            )
        )
    };
}

DirectX::XMFLOAT4 PanelText::SanitizeColor(
    const DirectX::XMFLOAT4& color
) noexcept
{
    return
    {
        ClampFloat(
            SanitizeFiniteValue(
                color.x,
                1.0f
            ),
            0.0f,
            1.0f
        ),

        ClampFloat(
            SanitizeFiniteValue(
                color.y,
                1.0f
            ),
            0.0f,
            1.0f
        ),

        ClampFloat(
            SanitizeFiniteValue(
                color.z,
                1.0f
            ),
            0.0f,
            1.0f
        ),

        ClampFloat(
            SanitizeFiniteValue(
                color.w,
                1.0f
            ),
            0.0f,
            1.0f
        )
    };
}

DirectX::XMFLOAT2 PanelText::SanitizeOffset(
    const DirectX::XMFLOAT2& offset
) noexcept
{
    return
    {
        SanitizeFiniteValue(
            offset.x,
            0.0f
        ),

        SanitizeFiniteValue(
            offset.y,
            0.0f
        )
    };
}

PanelTextStyle PanelText::SanitizeStyle(
    const PanelTextStyle& sourceStyle
) noexcept
{
    PanelTextStyle sanitizedStyle =
        sourceStyle;

    sanitizedStyle.textColor =
        SanitizeColor(
            sourceStyle.textColor
        );

    sanitizedStyle.minimumConfiguredScale =
        ClampFloat(
            SanitizeFiniteValue(
                sourceStyle.minimumConfiguredScale,
                0.05f
            ),
            AbsoluteMinimumScale,
            AbsoluteMaximumScale
        );

    sanitizedStyle.maximumScale =
        ClampFloat(
            SanitizeFiniteValue(
                sourceStyle.maximumScale,
                4.0f
            ),
            sanitizedStyle.minimumConfiguredScale,
            AbsoluteMaximumScale
        );

    sanitizedStyle.requestedScale =
        ClampFloat(
            SanitizeFiniteValue(
                sourceStyle.requestedScale,
                1.0f
            ),
            sanitizedStyle.minimumConfiguredScale,
            sanitizedStyle.maximumScale
        );

    sanitizedStyle.textOffset =
        SanitizeOffset(
            sourceStyle.textOffset
        );

    sanitizedStyle.spacesPerTab =
        (std::min)(
            sourceStyle.spacesPerTab,
            MaximumSpacesPerTab
        );

    sanitizedStyle.maximumCharacters =
        GetSafeMaximumCharacters(
            sourceStyle.maximumCharacters
        );

    return sanitizedStyle;
}

// =========================================================================================
// PANEL TEXT INPUT SANITIZATION
// =========================================================================================
std::wstring PanelText::SanitizeText(
    std::wstring_view sourceText
) const
{
    const std::size_t maximumCharacters =
        GetSafeMaximumCharacters(
            style.maximumCharacters
        );

    if (maximumCharacters == 0u ||
        sourceText.empty())
    {
        return {};
    }

    std::wstring sanitizedText;

    sanitizedText.reserve(
        (std::min)(
            sourceText.size(),
            maximumCharacters
        )
    );

    bool wasTruncated =
        false;

    for (const wchar_t character : sourceText)
    {
        if (sanitizedText.size() >=
            maximumCharacters)
        {
            wasTruncated =
                true;

            break;
        }

        if (character ==
            L'\0')
        {
            continue;
        }

        if (character ==
            L'\t')
        {
            const std::size_t availableCharacters =
                maximumCharacters -
                sanitizedText.size();

            const std::size_t spacesToAdd =
                (std::min)(
                    style.spacesPerTab,
                    availableCharacters
                );

            sanitizedText.append(
                spacesToAdd,
                L' '
            );

            continue;
        }

        if (character ==
            L'\r')
        {
            continue;
        }

        if (character ==
            L'\n')
        {
            sanitizedText.push_back(
                style.allowMultiline
                ? character
                : L' '
            );

            continue;
        }

        if (character >=
                static_cast<wchar_t>(
                    0x0001
                ) &&
            character <=
                static_cast<wchar_t>(
                    0x001F
                ))
        {
            continue;
        }

        if (character ==
            static_cast<wchar_t>(
                0x007F
            ))
        {
            continue;
        }

        sanitizedText.push_back(
            character
        );
    }

    if (wasTruncated &&
        style.showTruncationMarker &&
        maximumCharacters >= 3u)
    {
        sanitizedText.resize(
            (std::min)(
                sanitizedText.size(),
                maximumCharacters -
                    3u
            )
        );

        sanitizedText +=
            L"...";
    }

    return sanitizedText;
}

// =========================================================================================
// PANEL TEXT CHARACTER SUPPORT
// =========================================================================================
void PanelText::ReplaceUnsupportedCharacters(
    std::wstring& sourceText,
    DirectX::SpriteFont& spriteFont
)
{
    const wchar_t fallbackCharacter =
        spriteFont.ContainsCharacter(
            L'?'
        )
        ? L'?'
        : L' ';

    for (wchar_t& character : sourceText)
    {
        if (character ==
            L'\n')
        {
            continue;
        }

        if (!spriteFont.ContainsCharacter(
            character
        ))
        {
            character =
                fallbackCharacter;
        }
    }
}

// =========================================================================================
// PANEL TEXT MEASUREMENT
// =========================================================================================
bool PanelText::PrepareMeasuredText(
    DirectX::SpriteFont& spriteFont,
    std::wstring_view sourceText,
    std::wstring& preparedText,
    DirectX::XMFLOAT2& measuredSize
) const noexcept
{
    preparedText.clear();

    measuredSize =
    {
        0.0f,
        0.0f
    };

    try
    {
        preparedText =
            SanitizeText(
                sourceText
            );

        if (preparedText.empty())
        {
            return false;
        }

        ReplaceUnsupportedCharacters(
            preparedText,
            spriteFont
        );

        const DirectX::XMVECTOR measurement =
            spriteFont.MeasureString(
                preparedText.c_str()
            );

        measuredSize =
        {
            DirectX::XMVectorGetX(
                measurement
            ),

            DirectX::XMVectorGetY(
                measurement
            )
        };

        if (!std::isfinite(measuredSize.x) ||
            !std::isfinite(measuredSize.y) ||
            measuredSize.x <= 0.0f ||
            measuredSize.y <= 0.0f)
        {
            preparedText.clear();

            measuredSize =
            {
                0.0f,
                0.0f
            };

            return false;
        }

        return true;
    }
    catch (...)
    {
        preparedText.clear();

        measuredSize =
        {
            0.0f,
            0.0f
        };

        return false;
    }
}

// =========================================================================================
// PANEL TEXT PREPARED DRAWING
// =========================================================================================
bool PanelText::DrawPreparedText(
    DirectX::SpriteBatch& spriteBatch,
    DirectX::SpriteFont& spriteFont,
    const std::wstring& preparedText,
    const DirectX::XMFLOAT2& measuredSize
) const noexcept
{
    if (preparedText.empty() ||
        !std::isfinite(measuredSize.x) ||
        !std::isfinite(measuredSize.y) ||
        measuredSize.x <= 0.0f ||
        measuredSize.y <= 0.0f ||
        screenRectangle.width <= 0.0f ||
        screenRectangle.height <= 0.0f)
    {
        return false;
    }

    float scale =
        style.allowAutomaticUpscaling
        ? style.maximumScale
        : style.requestedScale;

    if (style.fitToWidth)
    {
        scale =
            (std::min)(
                scale,
                screenRectangle.width /
                    measuredSize.x
            );
    }

    if (style.fitToHeight)
    {
        scale =
            (std::min)(
                scale,
                screenRectangle.height /
                    measuredSize.y
            );
    }

    if (!std::isfinite(scale) ||
        scale <= 0.0f)
    {
        return false;
    }

    scale =
        (std::min)(
            scale,
            style.maximumScale
        );

    const float scaledWidth =
        measuredSize.x *
        scale;

    const float scaledHeight =
        measuredSize.y *
        scale;

    if (!std::isfinite(scaledWidth) ||
        !std::isfinite(scaledHeight))
    {
        return false;
    }

    float x =
        screenRectangle.x;

    float y =
        screenRectangle.y;

    switch (style.horizontalAlignment)
    {
    case PanelTextHorizontalAlignment::Left:
        break;

    case PanelTextHorizontalAlignment::Center:
        x +=
            (
                screenRectangle.width -
                scaledWidth
            ) *
            0.5f;
        break;

    case PanelTextHorizontalAlignment::Right:
        x +=
            screenRectangle.width -
            scaledWidth;
        break;
    }

    switch (style.verticalAlignment)
    {
    case PanelTextVerticalAlignment::Top:
        break;

    case PanelTextVerticalAlignment::Center:
        y +=
            (
                screenRectangle.height -
                scaledHeight
            ) *
            0.5f;
        break;

    case PanelTextVerticalAlignment::Bottom:
        y +=
            screenRectangle.height -
            scaledHeight;
        break;
    }

    x +=
        style.textOffset.x;

    y +=
        style.textOffset.y;

    if (!std::isfinite(x) ||
        !std::isfinite(y))
    {
        return false;
    }

    if (style.snapToPixels)
    {
        x =
            std::round(
                x
            );

        y =
            std::round(
                y
            );
    }

    const DirectX::XMVECTOR drawColor =
        DirectX::XMLoadFloat4(
            &style.textColor
        );

    try
    {
        spriteFont.DrawString(
            &spriteBatch,
            preparedText.c_str(),
            DirectX::XMFLOAT2(
                x,
                y
            ),
            drawColor,
            0.0f,
            DirectX::XMFLOAT2(
                0.0f,
                0.0f
            ),
            scale
        );

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// =========================================================================================
// PANEL TEXT CACHE
// =========================================================================================
void PanelText::InvalidateMeasurementCache() noexcept
{
    cachedFont =
        nullptr;

    cachedPreparedText.clear();

    cachedMeasuredSize =
    {
        0.0f,
        0.0f
    };

    measurementDirty =
        true;
}