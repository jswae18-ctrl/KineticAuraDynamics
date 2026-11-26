#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "../SphereMesh.h"
#include "../../Textures/Texture.h"

// =========================================================================================
// EARTH
// =========================================================================================
class Earth final
{
public:
    Earth(
        ID3D11Device* device,
        ID3D11DeviceContext* context
    );

    ~Earth() = default;

    Earth(const Earth&) = delete;
    Earth& operator=(const Earth&) = delete;
    Earth(Earth&&) = delete;
    Earth& operator=(Earth&&) = delete;

    void Update(float deltaTime) noexcept;

    void Draw(
        ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& worldViewProjection,
        const DirectX::XMMATRIX& world,
        const DirectX::XMFLOAT3& cameraPosition,
        const DirectX::XMFLOAT3& sunPosition
    );
    
    struct alignas(16) EarthLightingBuffer
    {
        DirectX::XMFLOAT3 sunPosition;
        float cloudOffset = 0.0f;
    };

    static_assert(
        sizeof(EarthLightingBuffer) % 16u == 0u,
        "Earth lighting buffer must be 16-byte aligned."
    );

private:
    SphereMesh sphere;

    Texture dayMap;
    Texture specularMap;
    Texture nightMap;
    Texture normalMap;
    Texture cloudMap;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> earthPixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> earthLightingBuffer;

    float cloudOffset = 0.0f;

    static constexpr float CloudMovementSpeed = 0.0025f;
};