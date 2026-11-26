#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "../SphereMesh.h"
#include "../../Textures/Texture.h"

// =========================================================================================
// SUN
// =========================================================================================
class Sun final
{
public:
    Sun(
        ID3D11Device* device,
        ID3D11DeviceContext* context
    );

    ~Sun() = default;

    Sun(const Sun&) = delete;
    Sun& operator=(const Sun&) = delete;
    Sun(Sun&&) = delete;
    Sun& operator=(Sun&&) = delete;

    void Draw(
        ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& worldViewProjection,
        const DirectX::XMMATRIX& world,
        const DirectX::XMFLOAT3& cameraPosition
    );

private:
    SphereMesh sphereMesh;
    Texture sunTexture;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> sunPixelShader;
};