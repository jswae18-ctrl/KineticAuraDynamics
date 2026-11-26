#pragma once

#include <d3d11.h>
#include <wrl/client.h>

// =========================================================================================
// HUD IMAGE PIPELINE
// =========================================================================================
class HudImagePipeline final
{
public:
    explicit HudImagePipeline(ID3D11Device* device);

    ~HudImagePipeline() = default;

    HudImagePipeline(const HudImagePipeline&) = delete;
    HudImagePipeline& operator=(const HudImagePipeline&) = delete;
    HudImagePipeline(HudImagePipeline&&) = delete;
    HudImagePipeline& operator=(HudImagePipeline&&) = delete;

    void Bind(
        ID3D11DeviceContext* context
    ) const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthDisabledState;
};