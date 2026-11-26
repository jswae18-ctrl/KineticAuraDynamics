cbuffer TransformationBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix worldMatrix;
    matrix normalMatrix;

    float3 cameraPosition;
    float padding;
};

// =========================================================================================
// VERTEX SHADER INPUT
// =========================================================================================

struct VS_INPUT
{
    float3 pos     : POSITION;
    float3 nrm     : NORMAL;
    float2 uv      : TEXTCOORD;
    float3 tangent : TANGENT;
};

// =========================================================================================
// VERTEX SHADER OUTPUT
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
// SPHERE VERTEX SHADER
// =========================================================================================

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(float4(input.pos, 1.0f), wvpMatrix);
    output.worldPos = mul(float4(input.pos, 1.0f), worldMatrix).xyz;

    output.nrm = normalize(mul(input.nrm, (float3x3)worldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));

    output.uv = input.uv;

    return output;
}