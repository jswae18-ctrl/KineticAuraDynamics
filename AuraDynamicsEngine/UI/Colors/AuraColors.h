#pragma once

#include <DirectXMath.h>

namespace AuraColors
{
    // =========================================================================
    // BASIC COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 White =
    { 1.00f, 1.00f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Black =
    { 0.00f, 0.00f, 0.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Transparent =
    { 0.00f, 0.00f, 0.00f, 0.00f };


    // =========================================================================
    // AURA HUD COLORS
    // =========================================================================

    // Main near-black blue background used by sci-fi panels.
    inline constexpr DirectX::XMFLOAT4 PanelDark =
    { 0.015f, 0.035f, 0.055f, 0.94f };

    // Slightly lighter panel body.
    inline constexpr DirectX::XMFLOAT4 PanelDarkSoft =
    { 0.025f, 0.055f, 0.075f, 0.92f };

    // Very dark background for terminal-style areas.
    inline constexpr DirectX::XMFLOAT4 TerminalDark =
    { 0.005f, 0.020f, 0.030f, 0.96f };

    // Dark cyan-tinted content region.
    inline constexpr DirectX::XMFLOAT4 ContentDark =
    { 0.015f, 0.070f, 0.085f, 0.90f };

    // Soft translucent cyan used for fake HUD glow.
    inline constexpr DirectX::XMFLOAT4 CyanGlowSoft =
    { 0.00f, 0.75f, 1.00f, 0.22f };

    // Soft translucent green glow.
    inline constexpr DirectX::XMFLOAT4 GreenGlowSoft =
    { 0.10f, 1.00f, 0.30f, 0.22f };

    // Soft translucent red glow.
    inline constexpr DirectX::XMFLOAT4 RedGlowSoft =
    { 1.00f, 0.10f, 0.12f, 0.22f };

    // Soft translucent amber glow.
    inline constexpr DirectX::XMFLOAT4 AmberGlowSoft =
    { 1.00f, 0.55f, 0.05f, 0.22f };
    
    
    // =========================================================================
    // CYAN COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Cyan =
    { 0.15f, 0.95f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CyanSoft =
    { 0.20f, 0.85f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CyanDark =
    { 0.05f, 0.16f, 0.22f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CyanButton =
    { 0.10f, 0.25f, 0.30f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CyanGlow =
    { 0.00f, 1.00f, 1.00f, 1.00f };


    // =========================================================================
    // BLUE COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Blue =
    { 0.00f, 0.30f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 BlueSoft =
    { 0.25f, 0.55f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 BlueDark =
    { 0.03f, 0.08f, 0.25f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 ElectricBlue =
    { 0.00f, 0.65f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 DeepBlue =
    { 0.02f, 0.05f, 0.15f, 1.00f };


    // =========================================================================
    // GREEN COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Green =
    { 0.00f, 1.00f, 0.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GreenSoft =
    { 0.20f, 0.85f, 0.35f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GreenDark =
    { 0.05f, 0.30f, 0.12f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 NeonGreen =
    { 0.20f, 1.00f, 0.35f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Lime =
    { 0.55f, 1.00f, 0.10f, 1.00f };


    // =========================================================================
    // RED COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Red =
    { 1.00f, 0.00f, 0.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 RedSoft =
    { 0.85f, 0.20f, 0.20f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 RedDark =
    { 0.35f, 0.05f, 0.07f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 RedGlow =
    { 1.00f, 0.15f, 0.10f, 1.00f };


    // =========================================================================
    // YELLOW AND ORANGE COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Yellow =
    { 1.00f, 1.00f, 0.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 YellowSoft =
    { 1.00f, 0.90f, 0.30f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Amber =
    { 1.00f, 0.65f, 0.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Orange =
    { 1.00f, 0.35f, 0.05f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 OrangeDark =
    { 0.50f, 0.15f, 0.03f, 1.00f };


    // =========================================================================
    // PURPLE AND MAGENTA COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Purple =
    { 0.55f, 0.20f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 PurpleSoft =
    { 0.70f, 0.45f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 PurpleDark =
    { 0.20f, 0.05f, 0.35f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 NeonPurple =
    { 0.75f, 0.20f, 1.00f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Magenta =
    { 1.00f, 0.10f, 0.85f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Pink =
    { 1.00f, 0.35f, 0.65f, 1.00f };

    // =========================================================================
    // GRAY COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Gray =
    { 0.50f, 0.50f, 0.50f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GrayLight =
    { 0.75f, 0.75f, 0.75f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GraySoft =
    { 0.62f, 0.62f, 0.62f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GrayDark =
    { 0.15f, 0.15f, 0.15f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GrayDeep =
    { 0.07f, 0.07f, 0.08f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Charcoal =
    { 0.10f, 0.11f, 0.12f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Slate =
    { 0.25f, 0.29f, 0.33f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SlateLight =
    { 0.42f, 0.48f, 0.53f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SlateDark =
    { 0.12f, 0.15f, 0.18f, 1.00f };


    // =========================================================================
    // SILVER COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Silver =
    { 0.75f, 0.75f, 0.78f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SilverLight =
    { 0.88f, 0.88f, 0.92f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SilverSoft =
    { 0.68f, 0.70f, 0.74f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SilverDark =
    { 0.38f, 0.40f, 0.43f, 1.00f };


    // =========================================================================
    // STEEL COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Steel =
    { 0.32f, 0.38f, 0.42f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SteelLight =
    { 0.55f, 0.62f, 0.67f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SteelBlue =
    { 0.25f, 0.38f, 0.48f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SteelDark =
    { 0.16f, 0.20f, 0.23f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 SteelDeep =
    { 0.08f, 0.11f, 0.13f, 1.00f };


    // =========================================================================
    // GUNMETAL AND GRAPHITE COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Gunmetal =
    { 0.12f, 0.16f, 0.18f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GunmetalLight =
    { 0.22f, 0.28f, 0.31f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GunmetalDark =
    { 0.06f, 0.09f, 0.11f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 Graphite =
    { 0.07f, 0.09f, 0.11f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GraphiteLight =
    { 0.18f, 0.21f, 0.24f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GraphiteDark =
    { 0.03f, 0.04f, 0.05f, 1.00f };


    // =========================================================================
    // TITANIUM COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Titanium =
    { 0.42f, 0.44f, 0.48f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 TitaniumLight =
    { 0.67f, 0.69f, 0.73f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 TitaniumBlue =
    { 0.32f, 0.39f, 0.47f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 TitaniumDark =
    { 0.22f, 0.24f, 0.28f, 1.00f };


    // =========================================================================
    // CHROME COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Chrome =
    { 0.82f, 0.84f, 0.88f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 ChromeLight =
    { 0.94f, 0.95f, 0.98f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 ChromeDark =
    { 0.45f, 0.48f, 0.52f, 1.00f };


    // =========================================================================
    // IRON COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Iron =
    { 0.28f, 0.29f, 0.30f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 IronLight =
    { 0.43f, 0.44f, 0.45f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 IronDark =
    { 0.12f, 0.13f, 0.14f, 1.00f };


    // =========================================================================
    // COPPER COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Copper =
    { 0.72f, 0.34f, 0.16f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CopperLight =
    { 0.90f, 0.52f, 0.28f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 CopperDark =
    { 0.38f, 0.15f, 0.07f, 1.00f };


    // =========================================================================
    // BRONZE COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Bronze =
    { 0.55f, 0.32f, 0.14f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 BronzeLight =
    { 0.72f, 0.46f, 0.22f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 BronzeDark =
    { 0.30f, 0.16f, 0.06f, 1.00f };


    // =========================================================================
    // GOLD COLORS
    // =========================================================================

    inline constexpr DirectX::XMFLOAT4 Gold =
    { 1.00f, 0.72f, 0.10f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GoldLight =
    { 1.00f, 0.86f, 0.38f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GoldSoft =
    { 0.82f, 0.62f, 0.20f, 1.00f };

    inline constexpr DirectX::XMFLOAT4 GoldDark =
    { 0.55f, 0.36f, 0.05f, 1.00f };
}