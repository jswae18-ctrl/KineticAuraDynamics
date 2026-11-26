// =========================================================================================
// HUD IMAGE VERTEX SHADER
// =========================================================================================

struct VS_INPUT
{
    float2 position : POSITION;
    float2 uv       : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position =
        float4(
            input.position,
            0.0f,
            1.0f
        );

    output.uv =
        input.uv;

    return output;
}
