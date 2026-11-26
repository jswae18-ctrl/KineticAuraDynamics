#include "PanelWindow.h"

#include "PanelImage.h"
#include "PanelText.h"

#include "../../Controls/PanelButton.h"
#include "../Textures/TextureCache.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
    PanelTextStyle CreatePanelTitleTextStyle(const PanelTitleStyle& titleStyle)
    {
        PanelTextStyle textStyle;

        textStyle.textColor = titleStyle.textColor;
        textStyle.requestedScale = titleStyle.textScale;
        textStyle.minimumConfiguredScale = 0.001f;
        textStyle.maximumScale = titleStyle.textScale;
        textStyle.textOffset = titleStyle.textOffset;

        textStyle.horizontalAlignment =
            titleStyle.centerHorizontally
            ? PanelTextHorizontalAlignment::Center
            : PanelTextHorizontalAlignment::Left;

        textStyle.verticalAlignment =
            titleStyle.centerVertically
            ? PanelTextVerticalAlignment::Center
            : PanelTextVerticalAlignment::Top;

        textStyle.fitToWidth = true;
        textStyle.fitToHeight = true;
        textStyle.allowAutomaticUpscaling = false;

        textStyle.allowMultiline = false;
        textStyle.maximumCharacters = 256u;
        textStyle.showTruncationMarker = true;

        textStyle.snapToPixels = true;
        textStyle.visible = titleStyle.visible;

        return textStyle;
    }

    LONG RoundToLong(float value) noexcept
    {
        if (!std::isfinite(value))
        {
            return 0L;
        }

        return static_cast<LONG>(std::lround(value));
    }
}

// =========================================================================================
// PANEL WINDOW CONSTRUCTION
// =========================================================================================
PanelWindow::PanelWindow(
    std::wstring title,
    PanelAnchor anchor,
    float normalWidth,
    float normalHeight,
    float minimizedHeight,
    float maximizedWidth,
    float maximizedHeight,
    PanelWindowOptions options,
    PanelWindowStyle style
)
    :
    title(std::move(title)),
    anchor(anchor),
    options(options),
    style(SanitizeStyle(style)),
    normalWidth(SanitizePositiveValue(normalWidth, 1.0f)),
    normalHeight(SanitizePositiveValue(normalHeight, 1.0f)),
    minimizedHeight(SanitizePositiveValue(minimizedHeight, 1.0f)),
    maximizedWidth(SanitizePositiveValue(maximizedWidth, normalWidth)),
    maximizedHeight(SanitizePositiveValue(maximizedHeight, normalHeight))
{
    if (!options.allowIndependentSizeControls &&
        options.canMinimize != options.canMaximize)
    {
        throw std::invalid_argument(
            "PanelWindow requires minimize and maximize to be enabled or disabled together."
        );
    }

    titleText = std::make_unique<PanelText>(
        this->style.title.rectangle,
        CreatePanelTitleTextStyle(this->style.title)
    );

    titleText->SetText(this->title);

    PanelButtonStyle windowButtonStyle;
    windowButtonStyle.backgroundColor = this->style.windowButtonBackgroundColor;
    windowButtonStyle.textColor = this->style.windowButtonTextColor;
    windowButtonStyle.textScale = 1.0f;

    PanelButtonStyle closeButtonStyle;
    closeButtonStyle.backgroundColor = this->style.closeButtonBackgroundColor;
    closeButtonStyle.textColor = this->style.closeButtonTextColor;
    closeButtonStyle.textScale = 1.0f;

    if (options.canMinimize)
    {
        minimizeButton = std::make_unique<PanelButton>(
            PanelRectangle{},
            L"-",
            PanelButtonType::Minimize,
            windowButtonStyle
        );

        restoreButton = std::make_unique<PanelButton>(
            PanelRectangle{},
            L"R",
            PanelButtonType::Restore,
            windowButtonStyle
        );
    }

    if (options.canMaximize)
    {
        maximizeButton = std::make_unique<PanelButton>(
            PanelRectangle{},
            L"+",
            PanelButtonType::Maximize,
            windowButtonStyle
        );
    }

    if (options.canClose)
    {
        closeButton = std::make_unique<PanelButton>(
            PanelRectangle{},
            L"x",
            PanelButtonType::Close,
            closeButtonStyle
        );
    }
}

PanelWindow::~PanelWindow() = default;

// =========================================================================================
// PANEL WINDOW VALUE SANITIZATION
// =========================================================================================
float PanelWindow::SanitizeFiniteValue(float value, float fallback) noexcept
{
    return std::isfinite(value) ? value : fallback;
}

float PanelWindow::SanitizePositiveValue(float value, float fallback) noexcept
{
    const float sanitizedFallback =
        std::isfinite(fallback) && fallback > 0.0f
        ? fallback
        : 1.0f;

    return
        std::isfinite(value) && value > 0.0f
        ? value
        : sanitizedFallback;
}

DirectX::XMFLOAT4 PanelWindow::SanitizeColor(
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

            return (std::max)(0.0f, (std::min)(value, 1.0f));
        };

    return
    {
        sanitizeComponent(color.x),
        sanitizeComponent(color.y),
        sanitizeComponent(color.z),
        sanitizeComponent(color.w)
    };
}

PanelRectangle PanelWindow::SanitizeRectangle(
    const PanelRectangle& rectangle
) noexcept
{
    return
    {
        SanitizeFiniteValue(rectangle.x, 0.0f),
        SanitizeFiniteValue(rectangle.y, 0.0f),
        (std::max)(0.0f, SanitizeFiniteValue(rectangle.width, 0.0f)),
        (std::max)(0.0f, SanitizeFiniteValue(rectangle.height, 0.0f))
    };
}

PanelWindowStyle PanelWindow::SanitizeStyle(
    const PanelWindowStyle& sourceStyle
) noexcept
{
    PanelWindowStyle sanitizedStyle = sourceStyle;

    sanitizedStyle.referenceScreenWidth =
        SanitizePositiveValue(sourceStyle.referenceScreenWidth, 1280.0f);

    sanitizedStyle.referenceScreenHeight =
        SanitizePositiveValue(sourceStyle.referenceScreenHeight, 720.0f);

    sanitizedStyle.minimumScale =
        SanitizePositiveValue(sourceStyle.minimumScale, 0.65f);

    sanitizedStyle.maximumScale =
        SanitizePositiveValue(sourceStyle.maximumScale, 2.50f);

    if (sanitizedStyle.maximumScale < sanitizedStyle.minimumScale)
    {
        sanitizedStyle.maximumScale = sanitizedStyle.minimumScale;
    }

    sanitizedStyle.placement.anchorOffset.x =
        SanitizeFiniteValue(sourceStyle.placement.anchorOffset.x, 0.0f);

    sanitizedStyle.placement.anchorOffset.y =
        SanitizeFiniteValue(sourceStyle.placement.anchorOffset.y, 0.0f);

    sanitizedStyle.title.rectangle =
        SanitizeRectangle(sourceStyle.title.rectangle);

    sanitizedStyle.title.textColor =
        SanitizeColor(sourceStyle.title.textColor);

    sanitizedStyle.title.textScale =
        SanitizePositiveValue(sourceStyle.title.textScale, 0.80f);

    sanitizedStyle.title.textOffset.x =
        SanitizeFiniteValue(sourceStyle.title.textOffset.x, 0.0f);

    sanitizedStyle.title.textOffset.y =
        SanitizeFiniteValue(sourceStyle.title.textOffset.y, 0.0f);

    sanitizedStyle.titleBarHeight =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.titleBarHeight, 48.0f)
        );

    sanitizedStyle.buttonSize =
        SanitizePositiveValue(sourceStyle.buttonSize, 18.0f);

    sanitizedStyle.buttonSpacing =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.buttonSpacing, 4.0f)
        );

    sanitizedStyle.buttonRightInset =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.buttonRightInset, 12.0f)
        );

    sanitizedStyle.normalButtonTopInset =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.normalButtonTopInset, 26.0f)
        );

    sanitizedStyle.minimizedButtonTopInset =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.minimizedButtonTopInset, 6.0f)
        );

    sanitizedStyle.windowButtonBackgroundColor =
        SanitizeColor(sourceStyle.windowButtonBackgroundColor);

    sanitizedStyle.windowButtonTextColor =
        SanitizeColor(sourceStyle.windowButtonTextColor);

    sanitizedStyle.closeButtonBackgroundColor =
        SanitizeColor(sourceStyle.closeButtonBackgroundColor);

    sanitizedStyle.closeButtonTextColor =
        SanitizeColor(sourceStyle.closeButtonTextColor);

    sanitizedStyle.normalHitTestTopInset =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.normalHitTestTopInset, 24.0f)
        );

    sanitizedStyle.minimizedHitTestTopInset =
        (std::max)(
            0.0f,
            SanitizeFiniteValue(sourceStyle.minimizedHitTestTopInset, 0.0f)
        );

    return sanitizedStyle;
}

// =========================================================================================
// PANEL WINDOW LAYOUT
// =========================================================================================
void PanelWindow::UpdateLayout(
    unsigned int newScreenWidth,
    unsigned int newScreenHeight
) noexcept
{
    screenWidth = newScreenWidth;
    screenHeight = newScreenHeight;

    RecalculateRectangle();
}

DirectX::XMFLOAT2 PanelWindow::CalculateScales() const noexcept
{
    if (style.scaleMode == PanelScaleMode::Fixed)
    {
        return
        {
            1.0f,
            1.0f
        };
    }

    if (screenWidth == 0u || screenHeight == 0u)
    {
        return
        {
            1.0f,
            1.0f
        };
    }

    const float scaleX =
        static_cast<float>(screenWidth) /
        style.referenceScreenWidth;

    const float scaleY =
        static_cast<float>(screenHeight) /
        style.referenceScreenHeight;

    float uniformScale = (std::min)(scaleX, scaleY);
    uniformScale = (std::max)(style.minimumScale, uniformScale);
    uniformScale = (std::min)(style.maximumScale, uniformScale);

    return
    {
        uniformScale,
        uniformScale
    };
}

DirectX::XMFLOAT2 PanelWindow::GetFullDesignSize() const noexcept
{
    switch (state)
    {
    case PanelWindowState::Normal:
        return
        {
            normalWidth,
            normalHeight
        };

    case PanelWindowState::Minimized:
        return
        {
            normalWidth,
            minimizedHeight
        };

    case PanelWindowState::Maximized:
        return
        {
            maximizedWidth,
            maximizedHeight
        };

    case PanelWindowState::Hidden:
        return {};
    }

    return {};
}

float PanelWindow::ScaleUniformValue(float designValue) const noexcept
{
    if (!std::isfinite(designValue))
    {
        return 0.0f;
    }

    const float scaledValue = designValue * GetUniformScale();

    return std::isfinite(scaledValue) ? scaledValue : 0.0f;
}

PanelRectangle PanelWindow::ScaleLocalRectangle(
    const PanelRectangle& localRectangle
) const noexcept
{
    const PanelRectangle rectangle = SanitizeRectangle(localRectangle);
    const float scale = GetUniformScale();

    return
    {
        rectangle.x * scale,
        rectangle.y * scale,
        rectangle.width * scale,
        rectangle.height * scale
    };
}

PanelRectangle PanelWindow::LocalToScreenRectangle(
    const PanelRectangle& localRectangle
) const noexcept
{
    const PanelRectangle rectangle = ScaleLocalRectangle(localRectangle);

    return
    {
        windowRectangle.x + rectangle.x,
        windowRectangle.y + rectangle.y,
        rectangle.width,
        rectangle.height
    };
}

PanelRectangle PanelWindow::CalculatePreferredRectangle(
    float activeWidth,
    float activeHeight,
    float activeMargin
) const noexcept
{
    float anchorX = 0.0f;
    float anchorY = 0.0f;

    float pivotX = 0.0f;
    float pivotY = 0.0f;

    switch (anchor)
    {
    case PanelAnchor::TopLeft:
        break;

    case PanelAnchor::TopRight:
        anchorX = 1.0f;
        pivotX = 1.0f;
        break;

    case PanelAnchor::BottomLeft:
        anchorY = 1.0f;
        pivotY = 1.0f;
        break;

    case PanelAnchor::BottomRight:
        anchorX = 1.0f;
        anchorY = 1.0f;

        pivotX = 1.0f;
        pivotY = 1.0f;
        break;

    case PanelAnchor::TopCenter:
        anchorX = 0.5f;
        pivotX = 0.5f;
        break;

    case PanelAnchor::BottomCenter:
        anchorX = 0.5f;
        anchorY = 1.0f;

        pivotX = 0.5f;
        pivotY = 1.0f;
        break;
    }

    const float viewportWidth = static_cast<float>(screenWidth);
    const float viewportHeight = static_cast<float>(screenHeight);

    float x =
        viewportWidth * anchorX -
        activeWidth * pivotX;

    float y =
        viewportHeight * anchorY -
        activeHeight * pivotY;

    if (anchorX <= 0.0f)
    {
        x += activeMargin;
    }
    else if (anchorX >= 1.0f)
    {
        x -= activeMargin;
    }

    if (anchorY <= 0.0f)
    {
        y += activeMargin;
    }
    else if (anchorY >= 1.0f)
    {
        y -= activeMargin;
    }

    x += ScaleUniformValue(style.placement.anchorOffset.x);
    y += ScaleUniformValue(style.placement.anchorOffset.y);

    return
    {
        std::isfinite(x) ? x : 0.0f,
        std::isfinite(y) ? y : 0.0f,
        activeWidth,
        activeHeight
    };
}

PanelRectangle PanelWindow::ClampRectangleToViewport(
    const PanelRectangle& rectangle,
    float activeMargin
) const noexcept
{
    const float viewportWidth = static_cast<float>(screenWidth);
    const float viewportHeight = static_cast<float>(screenHeight);

    const float minimumX = activeMargin;
    const float minimumY = activeMargin;

    const float maximumX =
        viewportWidth -
        rectangle.width -
        activeMargin;

    const float maximumY =
        viewportHeight -
        rectangle.height -
        activeMargin;

    PanelRectangle resolvedRectangle = rectangle;

    resolvedRectangle.x =
        (std::max)(
            minimumX,
            (std::min)(
                rectangle.x,
                maximumX
            )
        );

    resolvedRectangle.y =
        (std::max)(
            minimumY,
            (std::min)(
                rectangle.y,
                maximumY
            )
        );

    return resolvedRectangle;
}

void PanelWindow::RecalculateRectangle() noexcept
{
    const DirectX::XMFLOAT2 scales = CalculateScales();

    activeScaleX = scales.x;
    activeScaleY = scales.y;

    if (!IsUserVisible())
    {
        preferredRectangle = {};
        windowRectangle = {};
        fitsCurrentViewport = false;
        mouseInteractionStartedInside = false;
        return;
    }

    const DirectX::XMFLOAT2 designSize = GetFullDesignSize();

    const float activeWidth = ScaleUniformValue(designSize.x);
    const float activeHeight = ScaleUniformValue(designSize.y);
    const float activeMargin = ScaleUniformValue(ScreenMargin);

    const float viewportWidth = static_cast<float>(screenWidth);
    const float viewportHeight = static_cast<float>(screenHeight);

    if (!std::isfinite(activeWidth) ||
        !std::isfinite(activeHeight) ||
        !std::isfinite(activeMargin) ||
        activeWidth <= 0.0f ||
        activeHeight <= 0.0f ||
        activeMargin < 0.0f ||
        viewportWidth <= 0.0f ||
        viewportHeight <= 0.0f)
    {
        preferredRectangle = {};
        windowRectangle = {};
        fitsCurrentViewport = false;
        mouseInteractionStartedInside = false;
        return;
    }

    const float requiredWidth =
        activeWidth +
        activeMargin * 2.0f;

    const float requiredHeight =
        activeHeight +
        activeMargin * 2.0f;

    fitsCurrentViewport =
        std::isfinite(requiredWidth) &&
        std::isfinite(requiredHeight) &&
        viewportWidth >= requiredWidth &&
        viewportHeight >= requiredHeight;

    preferredRectangle =
        CalculatePreferredRectangle(
            activeWidth,
            activeHeight,
            activeMargin
        );

    if (!fitsCurrentViewport)
    {
        windowRectangle = {};
        mouseInteractionStartedInside = false;
        return;
    }

    windowRectangle =
        ClampRectangleToViewport(
            preferredRectangle,
            activeMargin
        );

    layoutVisible = true;

    RecalculateChildRectangles();
}

void PanelWindow::RecalculateChildRectangles() noexcept
{
    if (!IsVisible())
    {
        mouseInteractionStartedInside = false;
        return;
    }

    if (titleText)
    {
        titleText->SetScreenRectangle(GetTitleRectangle());
    }

    if (minimizeButton)
    {
        minimizeButton->SetRectangle(GetMinimizeButtonRectangle());
    }

    if (maximizeButton)
    {
        maximizeButton->SetRectangle(GetMaximizeButtonRectangle());
    }

    if (restoreButton)
    {
        restoreButton->SetRectangle(GetRestoreButtonRectangle());
    }

    if (closeButton)
    {
        closeButton->SetRectangle(GetCloseButtonRectangle());
    }

    if (IsMinimized())
    {
        return;
    }

    const PanelRectangle contentRectangle = GetContentRectangle();

    for (const auto& button : customButtons)
    {
        if (!button)
        {
            continue;
        }

        const PanelRectangle localRectangle =
            ScaleLocalRectangle(button->GetLocalRectangle());

        button->SetRectangle(
            {
                contentRectangle.x + localRectangle.x,
                contentRectangle.y + localRectangle.y,
                localRectangle.width,
                localRectangle.height
            }
        );
    }

    for (const auto& image : panelImages)
    {
        if (!image)
        {
            continue;
        }

        image->SetScreenRectangle(
            LocalToScreenRectangle(image->GetLocalRectangle())
        );
    }
}

// =========================================================================================
// PANEL WINDOW RESOLVED LAYOUT
// =========================================================================================
void PanelWindow::ApplyResolvedLayout(
    const PanelRectangle& rectangle,
    bool isVisible
) noexcept
{
    layoutVisible =
        isVisible &&
        IsUserVisible() &&
        fitsCurrentViewport;

    mouseInteractionStartedInside = false;

    if (!layoutVisible)
    {
        return;
    }

    const PanelRectangle sanitizedRectangle =
        SanitizeRectangle(rectangle);

    if (sanitizedRectangle.width <= 0.0f ||
        sanitizedRectangle.height <= 0.0f)
    {
        layoutVisible = false;
        return;
    }

    windowRectangle = sanitizedRectangle;

    RecalculateChildRectangles();
}

void PanelWindow::RestorePreferredLayout() noexcept
{
    layoutVisible =
        IsUserVisible() &&
        fitsCurrentViewport;

    mouseInteractionStartedInside = false;

    if (!layoutVisible)
    {
        return;
    }

    const float activeMargin = ScaleUniformValue(ScreenMargin);

    windowRectangle =
        ClampRectangleToViewport(
            preferredRectangle,
            activeMargin
        );

    RecalculateChildRectangles();
}

// =========================================================================================
// PANEL WINDOW GEOMETRY
// =========================================================================================
float PanelWindow::GetActiveTitleBarHeight() const noexcept
{
    return
        (std::min)(
            ScaleUniformValue(style.titleBarHeight),
            windowRectangle.height
        );
}

float PanelWindow::GetActiveButtonTopInset() const noexcept
{
    const float inset =
        state == PanelWindowState::Minimized
        ? style.minimizedButtonTopInset
        : style.normalButtonTopInset;

    return ScaleUniformValue(inset);
}

float PanelWindow::GetActiveHitTestTopInset() const noexcept
{
    const float inset =
        state == PanelWindowState::Minimized
        ? style.minimizedHitTestTopInset
        : style.normalHitTestTopInset;

    return ScaleUniformValue(inset);
}

PanelRectangle PanelWindow::GetTitleRectangle() const noexcept
{
    if (!IsVisible() || IsMinimized())
    {
        return {};
    }

    return LocalToScreenRectangle(style.title.rectangle);
}

PanelRectangle PanelWindow::GetContentRectangle() const noexcept
{
    if (!IsVisible() || IsMinimized())
    {
        return {};
    }

    const float titleBarHeight = GetActiveTitleBarHeight();
    const float contentPadding = ScaleUniformValue(ContentPadding);

    return
    {
        windowRectangle.x + contentPadding,
        windowRectangle.y + titleBarHeight,

        (std::max)(
            0.0f,
            windowRectangle.width -
            contentPadding * 2.0f
        ),

        (std::max)(
            0.0f,
            windowRectangle.height -
            titleBarHeight -
            contentPadding * 2.0f
        )
    };
}

PanelRectangle PanelWindow::GetButtonRectangle(
    unsigned int slotFromRight
) const noexcept
{
    if (!IsVisible())
    {
        return {};
    }

    const float buttonSize = ScaleUniformValue(style.buttonSize);
    const float buttonSpacing = ScaleUniformValue(style.buttonSpacing);
    const float rightInset = ScaleUniformValue(style.buttonRightInset);

    const float horizontalOffset =
        static_cast<float>(slotFromRight) *
        (buttonSize + buttonSpacing);

    return
    {
        windowRectangle.x +
        windowRectangle.width -
        rightInset -
        buttonSize -
        horizontalOffset,

        windowRectangle.y +
        GetActiveButtonTopInset(),

        buttonSize,
        buttonSize
    };
}

PanelRectangle PanelWindow::GetCloseButtonRectangle() const noexcept
{
    if (!options.canClose)
    {
        return {};
    }

    return GetButtonRectangle(0u);
}

PanelRectangle PanelWindow::GetRestoreButtonRectangle() const noexcept
{
    if (!restoreButton)
    {
        return {};
    }

    return GetButtonRectangle(options.canClose ? 1u : 0u);
}

PanelRectangle PanelWindow::GetMaximizeButtonRectangle() const noexcept
{
    if (!options.canMaximize)
    {
        return {};
    }

    return GetButtonRectangle(options.canClose ? 1u : 0u);
}

PanelRectangle PanelWindow::GetMinimizeButtonRectangle() const noexcept
{
    if (!options.canMinimize)
    {
        return {};
    }

    unsigned int slot = 0u;

    if (options.canClose)
    {
        ++slot;
    }

    if (options.canMaximize)
    {
        ++slot;
    }

    return GetButtonRectangle(slot);
}

// =========================================================================================
// PANEL WINDOW DRAWING
// =========================================================================================
void PanelWindow::Draw(
    DirectX::SpriteBatch* spriteBatch,
    DirectX::SpriteFont* spriteFont,
    ID3D11ShaderResourceView* whiteTexture,
    TextureCache& textureCache
) const
{
    if (!IsVisible() ||
        spriteBatch == nullptr ||
        spriteFont == nullptr ||
        whiteTexture == nullptr)
    {
        return;
    }

    if (!IsMinimized())
    {
        DrawPanelImages(*spriteBatch, textureCache);

        if (titleText)
        {
            titleText->Draw(spriteBatch, spriteFont);
        }
    }

    DrawBuiltInButtons(*spriteBatch, *spriteFont, whiteTexture);

    if (!IsMinimized())
    {
        DrawCustomButtons(*spriteBatch, *spriteFont, whiteTexture);
    }
}

void PanelWindow::DrawPanelImages(
    DirectX::SpriteBatch& spriteBatch,
    TextureCache& textureCache
) const
{
    for (const auto& image : panelImages)
    {
        if (!image)
        {
            continue;
        }

        ID3D11ShaderResourceView* imageTexture =
            textureCache.GetTexture(image->GetAssetPath());

        if (imageTexture == nullptr)
        {
            continue;
        }

        const TextureDimensions dimensions =
            textureCache.GetTextureDimensions(image->GetAssetPath());

        if (dimensions.width == 0u || dimensions.height == 0u)
        {
            continue;
        }

        const float textureWidth = static_cast<float>(dimensions.width);
        const float textureHeight = static_cast<float>(dimensions.height);

        PanelRectangle destinationRectangle = image->GetScreenRectangle();

        RECT sourceRectangle =
        {
            0L,
            0L,
            static_cast<LONG>(dimensions.width),
            static_cast<LONG>(dimensions.height)
        };

        const RECT* sourceRectanglePointer = nullptr;

        switch (image->GetStyle().mode)
        {
        case PanelImageMode::Native:
            destinationRectangle.width = ScaleUniformValue(textureWidth);
            destinationRectangle.height = ScaleUniformValue(textureHeight);
            break;

        case PanelImageMode::Fit:
        {
            if (destinationRectangle.width <= 0.0f ||
                destinationRectangle.height <= 0.0f)
            {
                continue;
            }

            const float imageScale =
                (std::min)(
                    destinationRectangle.width / textureWidth,
                    destinationRectangle.height / textureHeight
                );

            const float fittedWidth = textureWidth * imageScale;
            const float fittedHeight = textureHeight * imageScale;

            destinationRectangle.x +=
                (destinationRectangle.width - fittedWidth) * 0.5f;

            destinationRectangle.y +=
                (destinationRectangle.height - fittedHeight) * 0.5f;

            destinationRectangle.width = fittedWidth;
            destinationRectangle.height = fittedHeight;
            break;
        }

        case PanelImageMode::Fill:
        {
            if (destinationRectangle.width <= 0.0f ||
                destinationRectangle.height <= 0.0f)
            {
                continue;
            }

            const float destinationAspect =
                destinationRectangle.width /
                destinationRectangle.height;

            const float textureAspect =
                textureWidth /
                textureHeight;

            if (textureAspect > destinationAspect)
            {
                const float sourceWidth =
                    textureHeight *
                    destinationAspect;

                const float sourceLeft =
                    (textureWidth - sourceWidth) *
                    0.5f;

                sourceRectangle.left = RoundToLong(sourceLeft);
                sourceRectangle.right = RoundToLong(sourceLeft + sourceWidth);
            }
            else
            {
                const float sourceHeight =
                    textureWidth /
                    destinationAspect;

                const float sourceTop =
                    (textureHeight - sourceHeight) *
                    0.5f;

                sourceRectangle.top = RoundToLong(sourceTop);
                sourceRectangle.bottom = RoundToLong(sourceTop + sourceHeight);
            }

            sourceRectanglePointer = &sourceRectangle;
            break;
        }

        case PanelImageMode::Stretch:
            break;
        }

        if (!std::isfinite(destinationRectangle.x) ||
            !std::isfinite(destinationRectangle.y) ||
            !std::isfinite(destinationRectangle.width) ||
            !std::isfinite(destinationRectangle.height) ||
            destinationRectangle.width <= 0.0f ||
            destinationRectangle.height <= 0.0f)
        {
            continue;
        }

        const RECT destination =
        {
            RoundToLong(destinationRectangle.x),
            RoundToLong(destinationRectangle.y),

            RoundToLong(
                destinationRectangle.x +
                destinationRectangle.width
            ),

            RoundToLong(
                destinationRectangle.y +
                destinationRectangle.height
            )
        };

        if (destination.right <= destination.left ||
            destination.bottom <= destination.top)
        {
            continue;
        }

        spriteBatch.Draw(
            imageTexture,
            destination,
            sourceRectanglePointer
        );
    }
}

void PanelWindow::DrawBuiltInButtons(
    DirectX::SpriteBatch& spriteBatch,
    DirectX::SpriteFont& spriteFont,
    ID3D11ShaderResourceView* whiteTexture
) const
{
    if (state == PanelWindowState::Minimized)
    {
        if (restoreButton)
        {
            restoreButton->Draw(&spriteBatch, &spriteFont, whiteTexture);
        }
    }
    else
    {
        if (minimizeButton)
        {
            minimizeButton->Draw(&spriteBatch, &spriteFont, whiteTexture);
        }

        if (maximizeButton)
        {
            maximizeButton->Draw(&spriteBatch, &spriteFont, whiteTexture);
        }
    }

    if (closeButton)
    {
        closeButton->Draw(&spriteBatch, &spriteFont, whiteTexture);
    }
}

void PanelWindow::DrawCustomButtons(
    DirectX::SpriteBatch& spriteBatch,
    DirectX::SpriteFont& spriteFont,
    ID3D11ShaderResourceView* whiteTexture
) const
{
    for (const auto& button : customButtons)
    {
        if (button)
        {
            button->Draw(&spriteBatch, &spriteFont, whiteTexture);
        }
    }
}

// =========================================================================================
// PANEL WINDOW INPUT
// =========================================================================================
bool PanelWindow::ContainsPoint(float mouseX, float mouseY) const noexcept
{
    if (!IsVisible() ||
        !std::isfinite(mouseX) ||
        !std::isfinite(mouseY))
    {
        return false;
    }

    const float topInset =
        (std::min)(
            GetActiveHitTestTopInset(),
            windowRectangle.height
        );

    const PanelRectangle interactiveRectangle =
    {
        windowRectangle.x,
        windowRectangle.y + topInset,
        windowRectangle.width,

        (std::max)(
            0.0f,
            windowRectangle.height - topInset
        )
    };

    return interactiveRectangle.Contains(mouseX, mouseY);
}

bool PanelWindow::WantsMouseCapture(const Mouse& mouse) const noexcept
{
    if (!IsVisible() || !options.blocksMouseInput)
    {
        return false;
    }

    if (!options.blockWhileHovering)
    {
        return mouseInteractionStartedInside;
    }

    return ContainsPoint(
        static_cast<float>(mouse.GetPosX()),
        static_cast<float>(mouse.GetPosY())
    );
}

bool PanelWindow::HandleMouseEvent(const Mouse::Event& event) noexcept
{
    if (!IsVisible())
    {
        mouseInteractionStartedInside = false;
        return false;
    }

    const float mouseX = static_cast<float>(event.GetPosX());
    const float mouseY = static_cast<float>(event.GetPosY());
    const Mouse::Event::Type eventType = event.GetType();

    const bool containsMouse = ContainsPoint(mouseX, mouseY);

    if (eventType == Mouse::Event::Type::LPress)
    {
        mouseInteractionStartedInside = containsMouse;
    }
    else if (eventType == Mouse::Event::Type::LRelease)
    {
        mouseInteractionStartedInside = false;
    }

    if (eventType != Mouse::Event::Type::LPress)
    {
        return containsMouse && options.blocksMouseInput;
    }

    if (closeButton &&
        closeButton->ContainsPoint(mouseX, mouseY))
    {
        Hide();
        return true;
    }

    if (state == PanelWindowState::Minimized)
    {
        if (restoreButton &&
            restoreButton->ContainsPoint(mouseX, mouseY))
        {
            Restore();
            return true;
        }
    }
    else
    {
        if (maximizeButton &&
            maximizeButton->ContainsPoint(mouseX, mouseY))
        {
            if (state == PanelWindowState::Maximized)
            {
                Restore();
            }
            else
            {
                Maximize();
            }

            return true;
        }

        if (minimizeButton &&
            minimizeButton->ContainsPoint(mouseX, mouseY))
        {
            Minimize();
            return true;
        }
    }

    if (!containsMouse)
    {
        return false;
    }

    if (!IsMinimized())
    {
        for (const auto& button : customButtons)
        {
            if (!button ||
                !button->ContainsPoint(mouseX, mouseY))
            {
                continue;
            }

            try
            {
                button->Invoke();
            }
            catch (...)
            {
                OutputDebugStringA(
                    "PanelWindow custom button action threw an exception.\n"
                );
            }

            return true;
        }
    }

    return options.blocksMouseInput;
}

// =========================================================================================
// PANEL WINDOW ELEMENT CREATION
// =========================================================================================
void PanelWindow::AddButton(
    const PanelRectangle& rectangle,
    std::wstring text,
    PanelButtonStyle buttonStyle,
    std::function<void()> action
)
{
    customButtons.push_back(
        std::make_unique<PanelButton>(
            SanitizeRectangle(rectangle),
            std::move(text),
            PanelButtonType::Custom,
            buttonStyle,
            std::move(action)
        )
    );

    RecalculateChildRectangles();
}

void PanelWindow::AddImage(
    std::wstring assetPath,
    const PanelRectangle& rectangle,
    PanelImageStyle imageStyle
)
{
    if (assetPath.empty())
    {
        throw std::invalid_argument(
            "PanelWindow requires a non-empty image asset path."
        );
    }

    panelImages.push_back(
        std::make_unique<PanelImage>(
            std::move(assetPath),
            SanitizeRectangle(rectangle),
            imageStyle
        )
    );

    RecalculateChildRectangles();
}

// =========================================================================================
// PANEL WINDOW STATE
// =========================================================================================
void PanelWindow::Minimize() noexcept
{
    if (!options.canMinimize ||
        state == PanelWindowState::Minimized ||
        state == PanelWindowState::Hidden)
    {
        return;
    }

    state = PanelWindowState::Minimized;
    mouseInteractionStartedInside = false;

    RecalculateRectangle();
}

void PanelWindow::Maximize() noexcept
{
    if (!options.canMaximize ||
        state == PanelWindowState::Maximized ||
        state == PanelWindowState::Hidden)
    {
        return;
    }

    state = PanelWindowState::Maximized;
    mouseInteractionStartedInside = false;

    RecalculateRectangle();
}

void PanelWindow::Restore() noexcept
{
    if (state == PanelWindowState::Hidden ||
        state == PanelWindowState::Normal)
    {
        return;
    }

    state = PanelWindowState::Normal;
    mouseInteractionStartedInside = false;

    RecalculateRectangle();
}

void PanelWindow::Hide() noexcept
{
    if (state == PanelWindowState::Hidden)
    {
        return;
    }

    state = PanelWindowState::Hidden;

    preferredRectangle = {};
    windowRectangle = {};

    layoutVisible = false;
    fitsCurrentViewport = false;
    mouseInteractionStartedInside = false;
}

void PanelWindow::Show() noexcept
{
    if (state == PanelWindowState::Normal)
    {
        return;
    }

    state = PanelWindowState::Normal;
    layoutVisible = true;
    mouseInteractionStartedInside = false;

    RecalculateRectangle();
}