// =========================================================================================
// HUD PIXEL SHADER
// =========================================================================================

Texture2D hudTexture : register(t0);
SamplerState hudSampler : register(s0);

struct VSOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

float4 main(VSOutput input) : SV_Target
{
    return hudTexture.Sample(
        hudSampler,
        input.uv
    );
}