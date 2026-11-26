#include "PanelImage.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

// =========================================================================================
// PANEL IMAGE CONSTRUCTION
// =========================================================================================
PanelImage::PanelImage(
    std::wstring assetPath,
    const PanelRectangle& localRectangle,
    PanelImageStyle style
)
    :
    assetPath(std::move(assetPath)),
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
    style(style)
{
    if (this->assetPath.empty())
    {
        throw std::invalid_argument(
            "PanelImage requires a non-empty asset path."
        );
    }
}

// =========================================================================================
// PANEL IMAGE RECTANGLES
// =========================================================================================
void PanelImage::SetLocalRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    localRectangle =
        SanitizeRectangle(
            rectangle
        );
}

void PanelImage::SetScreenRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    screenRectangle =
        SanitizeRectangle(
            rectangle
        );
}

// =========================================================================================
// PANEL IMAGE STYLE
// =========================================================================================
void PanelImage::SetStyle(
    PanelImageStyle newStyle
) noexcept
{
    style =
        newStyle;
}

// =========================================================================================
// PANEL IMAGE VALUE SANITIZATION
// =========================================================================================
float PanelImage::SanitizeFiniteValue(
    float value,
    float fallback
) noexcept
{
    return std::isfinite(value)
        ? value
        : fallback;
}

PanelRectangle PanelImage::SanitizeRectangle(
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