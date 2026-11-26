#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "HudImagePipeline.h"

// =========================================================================================
// HUD IMAGE RENDERER
// =========================================================================================
class HudImageRenderer final
{
public:
    HudImageRenderer(
        ID3D11Device* device,
        HudImagePipeline& pipeline,
        ID3D11ShaderResourceView* textureView,
        const DirectX::XMFLOAT4& tintColor
    );

    ~HudImageRenderer() = default;

    HudImageRenderer(const HudImageRenderer&) = delete;
    HudImageRenderer& operator=(const HudImageRenderer&) = delete;
    HudImageRenderer(HudImageRenderer&&) = delete;
    HudImageRenderer& operator=(HudImageRenderer&&) = delete;

    void Draw(
        ID3D11DeviceContext* context,
        float left,
        float top,
        float width,
        float height,
        unsigned int screenWidth,
        unsigned int screenHeight
    );

    void SetTint(
        const DirectX::XMFLOAT4& color
    ) noexcept;

private:
    struct HudVertex
    {
        DirectX::XMFLOAT2 position;
        DirectX::XMFLOAT2 textureCoordinate;
    };

    struct HudColorBuffer
    {
        DirectX::XMFLOAT4 tintColor;
    };

    void UpdateVertices(
        ID3D11DeviceContext* context,
        float left,
        float top,
        float width,
        float height,
        unsigned int screenWidth,
        unsigned int screenHeight
    );

    void UpdateColorBuffer(
        ID3D11DeviceContext* context
    );

private:
    // The shared pipeline is owned by HUD and must outlive this renderer.
    HudImagePipeline& pipeline;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> colorBuffer;

    DirectX::XMFLOAT4 tintColor;
};