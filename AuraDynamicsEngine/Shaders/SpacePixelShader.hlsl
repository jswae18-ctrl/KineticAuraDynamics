// =========================================================================================
// SPACE PIXEL SHADER
// =========================================================================================

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
// SPACE TEXTURES
// =========================================================================================

// Primary distant star field.
Texture2D StarTexture :
    register(t0);

// Subtle Milky Way galactic background.
Texture2D MilkyWayTexture :
    register(t1);

SamplerState StarSampler :
    register(s0);

// =========================================================================================
// SPACE PIXEL SHADER
// =========================================================================================

float4 main(
    VS_OUTPUT input
) : SV_TARGET
{
    // -------------------------------------------------------------------------
    // FIXED CELESTIAL SPHERE UV
    // -------------------------------------------------------------------------

    float2 starUV =
        input.uv;

    // -------------------------------------------------------------------------
    // PRIMARY STAR FIELD
    // -------------------------------------------------------------------------

    float3 starColor =
        StarTexture.Sample(
            StarSampler,
            starUV
        ).rgb;

    // -------------------------------------------------------------------------
    // MILKY WAY LAYER
    // -------------------------------------------------------------------------

    float3 milkyWayColor =
        MilkyWayTexture.Sample(
            StarSampler,
            starUV
        ).rgb;

    // Milky Way remains intentionally subtle so the additional
    // stars do not overwhelm the primary star field.

    const float milkyWayStrength =
        0.5f;

    milkyWayColor *=
        milkyWayStrength;

    // -------------------------------------------------------------------------
    // COMBINE DEEP SPACE LAYERS
    // -------------------------------------------------------------------------

    float3 finalSpaceColor =
        starColor +
        milkyWayColor;

    finalSpaceColor =
        saturate(
            finalSpaceColor
        );

    return float4(
        finalSpaceColor,
        1.0f
    );
}