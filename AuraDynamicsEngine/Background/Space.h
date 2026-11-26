#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "../Assets/SphereMesh.h"
#include "../Textures/Texture.h"

// =========================================================================================
// SPACE
// =========================================================================================
class Space final
{
public:
    Space(
        ID3D11Device* device,
        ID3D11DeviceContext* context
    );

    ~Space() = default;

    Space(const Space&) = delete;
    Space& operator=(const Space&) = delete;
    Space(Space&&) = delete;
    Space& operator=(Space&&) = delete;

    void Draw(
        ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection,
        const DirectX::XMFLOAT3& cameraPosition
    );

private:
    SphereMesh spaceSphere;

    Texture starTexture;
    Texture milkyWayTexture;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> spacePixelShader;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> insideRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> backgroundDepthState;

    static constexpr float SpaceSphereScale = 50.0f;
};