cbuffer TransformationBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix worldMatrix;
    matrix normalMatrix;

    float3 cameraPosition;
    float padding;
};

// =========================================================================================
// PIXEL SHADER INPUT
// =========================================================================================

struct VS_OUTPUT
{
    float4 pos      : SV_POSITION;
    float3 worldPos : POSITION;
    float3 nrm      : NORMAL;
    float2 uv       : TEXTCOORD;
    float3 tangent  : TANGENT;
};

// =========================================================================================
// SUN SURFACE TEXTURE
// =========================================================================================

Texture2D SunTexture    : register(t0);
SamplerState SunSampler : register(s0);

// =========================================================================================
// SUN PIXEL SHADER
// =========================================================================================

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // ---------------------------------------------------------
    // SAMPLE SOLAR PHOTOSPHERE TEXTURE
    // ---------------------------------------------------------

    float3 surfaceColor = SunTexture.Sample(SunSampler, input.uv).rgb;

    // ---------------------------------------------------------
    // EMISSIVE SOLAR COLOR
    // ---------------------------------------------------------

    const float3 warmSolarTint = float3(1.0f, 0.82f, 0.52f);
    const float surfaceBrightness = 1.30f;

    float3 finalColor = surfaceColor * warmSolarTint * surfaceBrightness;

    // Compress the brightness into the current SDR render-target range.
    finalColor = finalColor / (finalColor + 1.0f);

    return float4(finalColor, 1.0f);
}