#pragma once
#include <string>
#include "PanelWindow.h"

// =========================================================================================
// PANEL IMAGE
// =========================================================================================
class PanelImage final
{
public:
    PanelImage(
        std::wstring assetPath,
        const PanelRectangle& localRectangle,
        PanelImageStyle style = {}
    );

    ~PanelImage() = default;

    PanelImage(const PanelImage&) = default;
    PanelImage& operator=(const PanelImage&) = default;
    PanelImage(PanelImage&&) noexcept = default;
    PanelImage& operator=(PanelImage&&) noexcept = default;

    const std::wstring& GetAssetPath() const noexcept
    {
        return assetPath;
    }

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

    const PanelImageStyle& GetStyle() const noexcept
    {
        return style;
    }

    void SetStyle(
        PanelImageStyle newStyle
    ) noexcept;

private:
    static float SanitizeFiniteValue(
        float value,
        float fallback
    ) noexcept;

    static PanelRectangle SanitizeRectangle(
        const PanelRectangle& rectangle
    ) noexcept;

private:
    std::wstring assetPath;

    PanelRectangle localRectangle;
    PanelRectangle screenRectangle;

    PanelImageStyle style;
};