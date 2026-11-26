// =========================================================================================
// HUD IMAGE PIXEL SHADER
// =========================================================================================

cbuffer HudColorBuffer : register(b0)
{
    float4 TintColor;
};


Texture2D HudTexture : register(t0);

SamplerState HudSampler : register(s0);


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    const float4 textureColor =
        HudTexture.Sample(
            HudSampler,
            input.uv
        );


    const float brightness =
        max(
            textureColor.r,
            max(
                textureColor.g,
                textureColor.b
            )
        );


    const float3 finalColor =
        TintColor.rgb *
        brightness;


    return float4(
        finalColor,
        textureColor.a *
        TintColor.a
    );
}