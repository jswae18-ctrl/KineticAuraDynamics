#pragma once
#include <cstdint>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

// =========================================================================================
// SPHERE MESH
// =========================================================================================
class SphereMesh final
{
public:
    struct SphereVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 textureCoordinate;
        DirectX::XMFLOAT3 tangent;
    };

    struct alignas(16) TransformationBuffer
    {
        DirectX::XMMATRIX worldViewProjection;
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX normal;
        DirectX::XMFLOAT3 cameraPosition;
        float padding = 0.0f;
    };

    static_assert(
        sizeof(TransformationBuffer) % 16u == 0u,
        "SphereMesh transformation buffer must be 16-byte aligned."
    );

    explicit SphereMesh(ID3D11Device* device);

    ~SphereMesh() = default;

    SphereMesh(const SphereMesh&) = delete;
    SphereMesh& operator=(const SphereMesh&) = delete;
    SphereMesh(SphereMesh&&) = delete;
    SphereMesh& operator=(SphereMesh&&) = delete;

    void Draw(
        ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& worldViewProjection,
        const DirectX::XMMATRIX& world,
        const DirectX::XMFLOAT3& cameraPosition
    );

private:
    static void BuildSphere(
        float radius,
        std::uint32_t slices,
        std::uint32_t stacks,
        std::vector<SphereVertex>& vertices,
        std::vector<std::uint32_t>& indices
    );

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> transformationBuffer;

    UINT indexCount = 0u;

    static constexpr float SphereRadius = 1.0f;
    static constexpr std::uint32_t SphereSlices = 128u;
    static constexpr std::uint32_t SphereStacks = 64u;
};