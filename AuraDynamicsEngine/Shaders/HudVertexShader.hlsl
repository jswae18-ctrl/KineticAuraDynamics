cbuffer VSConstantBuffer : register(b0)
{
    matrix orbitMatrix;     // 64 Bytes: Discarded automatically during HUD draw passes
    float4 instanceColor;   // 16 Bytes
}; // Total Footprint = 80 Bytes [Contract C]

// ⚡ REGISTER SLOT B1: Automatically handles incoming clean-room cursor parameters
cbuffer HUDParameterBuffer : register(b1)
{
    float2 cursorOffsetNDC;   // 8 Bytes: Continuous clean-room mouse offsets (X, Y)
    float2 structuralPadding; // 8 Bytes
}; // Total Footprint = 16 Bytes. Synchronized character-for-character with your C++ [eRDy9k]

struct VSInput  { float2 pos : POSITION; };
struct VSOutput { float2 uv : TEXCOORD0; float4 color : COLOR; float4 pos : SV_Position; };

VSOutput main(VSInput input)
{
    VSOutput output;
    float2 transformedPos = input.pos;

    // ⚡ RETICLE SEPARATION GATE: Only translate the inner targeting reticle elements
    // Any coordinate layout within 0.15f of the center follows your mouse cursor perfectly
    if (abs(input.pos.x) < 0.15f && abs(input.pos.y) < 0.15f)
    {
        // Add the clean, un-transposed offset vector to match your mouse position
        transformedPos.x += cursorOffsetNDC.x;
        transformedPos.y += cursorOffsetNDC.y;
    }

    output.pos = float4(transformedPos, 0.0f, 1.0f);
    output.color = float4(0.0f, 1.0f, 0.2f, 1.0f); // High-visibility tactical green
    output.uv = (input.pos + 1.0f) * 0.5f;
    
    return output;
}