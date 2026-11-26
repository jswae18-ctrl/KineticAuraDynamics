#include "SphereMesh.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <iterator>

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
    // =====================================================================================
    // DIRECT3D VALIDATION
    // =====================================================================================
    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "SphereMesh requires a valid Direct3D device."
            );
        }

        return device;
    }

    void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
        {
            throw std::runtime_error(message);
        }
    }
    
    UINT CalculateBufferByteWidth(
    std::size_t elementSize,
    std::size_t elementCount,
    const char* errorMessage
)
    {
        if (elementSize == 0u ||
            elementCount == 0u ||
            elementCount > (std::numeric_limits<UINT>::max)() / elementSize)
        {
            throw std::length_error(errorMessage);
        }

        return static_cast<UINT>(
            elementSize * elementCount
        );
    }
    bool IsFinitePosition(const DirectX::XMFLOAT3& position) noexcept
    {
        return
            std::isfinite(position.x) &&
            std::isfinite(position.y) &&
            std::isfinite(position.z);
    }

    // =====================================================================================
    // VERTEX SHADER COMPILATION
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3DBlob> CompileSphereVertexShader()
    {
        const std::wstring shaderPath =
            L"Shaders\\VertexShader.hlsl";

        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
        compileFlags |=
            D3DCOMPILE_DEBUG |
            D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBytecode;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        const HRESULT result =
            D3DCompileFromFile(
                shaderPath.c_str(),
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main",
                "vs_5_0",
                compileFlags,
                0u,
                shaderBytecode.GetAddressOf(),
                errorBlob.GetAddressOf()
            );

        if (FAILED(result))
        {
            if (errorBlob &&
                errorBlob->GetBufferPointer() != nullptr)
            {
                OutputDebugStringA(
                    static_cast<const char*>(
                        errorBlob->GetBufferPointer()
                    )
                );
            }

            throw std::runtime_error(
                "Failed to compile the SphereMesh vertex shader."
            );
        }

        if (!shaderBytecode ||
            shaderBytecode->GetBufferPointer() == nullptr ||
            shaderBytecode->GetBufferSize() == 0u)
        {
            throw std::runtime_error(
                "SphereMesh vertex shader compilation returned no bytecode."
            );
        }

        return shaderBytecode;
    }

    // =====================================================================================
    // STATIC BUFFER CREATION
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(
        ID3D11Device* device,
        const std::vector<SphereMesh::SphereVertex>& vertices
    )
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth =
            CalculateBufferByteWidth(
                sizeof(SphereMesh::SphereVertex),
                vertices.size(),
                "SphereMesh vertex data exceeds the supported buffer size."
            );

        description.Usage = D3D11_USAGE_IMMUTABLE;
        description.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = vertices.data();

        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

        ThrowIfFailed(
            device->CreateBuffer(
                &description,
                &initialData,
                buffer.GetAddressOf()
            ),
            "Failed to create the SphereMesh vertex buffer."
        );

        return buffer;
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(
        ID3D11Device* device,
        const std::vector<std::uint32_t>& indices
    )
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth =
            CalculateBufferByteWidth(
                sizeof(std::uint32_t),
                indices.size(),
                "SphereMesh index data exceeds the supported buffer size."
            );

        description.Usage = D3D11_USAGE_IMMUTABLE;
        description.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = indices.data();

        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

        ThrowIfFailed(
            device->CreateBuffer(
                &description,
                &initialData,
                buffer.GetAddressOf()
            ),
            "Failed to create the SphereMesh index buffer."
        );

        return buffer;
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateTransformationBuffer(
        ID3D11Device* device
    )
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth = sizeof(SphereMesh::TransformationBuffer);
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

        ThrowIfFailed(
            device->CreateBuffer(
                &description,
                nullptr,
                buffer.GetAddressOf()
            ),
            "Failed to create the SphereMesh transformation buffer."
        );

        return buffer;
    }
}

// =========================================================================================
// SPHERE GEOMETRY
// =========================================================================================
void SphereMesh::BuildSphere(
    float radius,
    std::uint32_t slices,
    std::uint32_t stacks,
    std::vector<SphereVertex>& vertices,
    std::vector<std::uint32_t>& indices
)
{
    if (!std::isfinite(radius) ||
        radius <= 0.0f)
    {
        throw std::invalid_argument(
            "SphereMesh radius must be finite and greater than zero."
        );
    }

    if (slices < 3u)
    {
        throw std::invalid_argument(
            "SphereMesh requires at least three slices."
        );
    }

    if (stacks < 2u)
    {
        throw std::invalid_argument(
            "SphereMesh requires at least two stacks."
        );
    }

    const std::uint64_t vertexCount =
        static_cast<std::uint64_t>(slices + 1u) *
        static_cast<std::uint64_t>(stacks + 1u);

    const std::uint64_t indexCount =
        static_cast<std::uint64_t>(slices) *
        static_cast<std::uint64_t>(stacks) *
        6u;

    if (vertexCount > (std::numeric_limits<std::uint32_t>::max)() ||
        indexCount > (std::numeric_limits<std::uint32_t>::max)())
    {
        throw std::length_error(
            "SphereMesh geometry exceeds 32-bit index limits."
        );
    }

    vertices.clear();
    indices.clear();

    vertices.reserve(
        static_cast<std::size_t>(vertexCount)
    );

    indices.reserve(
        static_cast<std::size_t>(indexCount)
    );

    for (std::uint32_t stack = 0u; stack <= stacks; ++stack)
    {
        const float verticalCoordinate =
            static_cast<float>(stack) /
            static_cast<float>(stacks);

        const float polarAngle =
            DirectX::XM_PI *
            verticalCoordinate;

        const float polarSine = std::sin(polarAngle);
        const float polarCosine = std::cos(polarAngle);

        for (std::uint32_t slice = 0u; slice <= slices; ++slice)
        {
            const float horizontalCoordinate =
                static_cast<float>(slice) /
                static_cast<float>(slices);

            const float azimuthAngle =
                DirectX::XM_2PI *
                horizontalCoordinate;

            const float azimuthSine = std::sin(azimuthAngle);
            const float azimuthCosine = std::cos(azimuthAngle);

            const DirectX::XMFLOAT3 normal =
            {
                polarSine * azimuthCosine,
                polarCosine,
                polarSine * azimuthSine
            };

            SphereVertex vertex;

            vertex.position =
            {
                normal.x * radius,
                normal.y * radius,
                normal.z * radius
            };

            vertex.normal = normal;

            vertex.textureCoordinate =
            {
                horizontalCoordinate,
                verticalCoordinate
            };

            vertex.tangent =
            {
                -azimuthSine,
                0.0f,
                azimuthCosine
            };

            vertices.push_back(vertex);
        }
    }

    const std::uint32_t rowWidth = slices + 1u;

    for (std::uint32_t stack = 0u; stack < stacks; ++stack)
    {
        for (std::uint32_t slice = 0u; slice < slices; ++slice)
        {
            const std::uint32_t topLeft =
                stack * rowWidth +
                slice;

            const std::uint32_t bottomLeft =
                topLeft +
                rowWidth;

            indices.push_back(topLeft);
            indices.push_back(topLeft + 1u);
            indices.push_back(bottomLeft);

            indices.push_back(topLeft + 1u);
            indices.push_back(bottomLeft + 1u);
            indices.push_back(bottomLeft);
        }
    }
}

// =========================================================================================
// SPHERE MESH CONSTRUCTION
// =========================================================================================
SphereMesh::SphereMesh(ID3D11Device* device)
{
    device = ValidateDevice(device);

    const Microsoft::WRL::ComPtr<ID3DBlob> shaderBytecode =
        CompileSphereVertexShader();

    ThrowIfFailed(
        device->CreateVertexShader(
            shaderBytecode->GetBufferPointer(),
            shaderBytecode->GetBufferSize(),
            nullptr,
            vertexShader.GetAddressOf()
        ),
        "Failed to create the SphereMesh vertex shader."
    );

    const D3D11_INPUT_ELEMENT_DESC inputElements[] =
    {
        {
            "POSITION",
            0u,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0u,
            static_cast<UINT>(offsetof(SphereVertex, position)),
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        },
        {
            "NORMAL",
            0u,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0u,
            static_cast<UINT>(offsetof(SphereVertex, normal)),
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        },
        {
            "TEXTCOORD",
            0u,
            DXGI_FORMAT_R32G32_FLOAT,
            0u,
            static_cast<UINT>(offsetof(SphereVertex, textureCoordinate)),
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        },
        {
            "TANGENT",
            0u,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0u,
            static_cast<UINT>(offsetof(SphereVertex, tangent)),
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        }
    };

    ThrowIfFailed(
        device->CreateInputLayout(
            inputElements,
            static_cast<UINT>(std::size(inputElements)),
            shaderBytecode->GetBufferPointer(),
            shaderBytecode->GetBufferSize(),
            inputLayout.GetAddressOf()
        ),
        "Failed to create the SphereMesh input layout."
    );

    std::vector<SphereVertex> vertices;
    std::vector<std::uint32_t> indices;

    BuildSphere(
        SphereRadius,
        SphereSlices,
        SphereStacks,
        vertices,
        indices
    );

    if (indices.size() > (std::numeric_limits<UINT>::max)())
    {
        throw std::length_error(
            "SphereMesh index count exceeds the supported draw range."
        );
    }

    indexCount = static_cast<UINT>(indices.size());

    vertexBuffer = CreateVertexBuffer(device, vertices);
    indexBuffer = CreateIndexBuffer(device, indices);
    transformationBuffer = CreateTransformationBuffer(device);
}

// =========================================================================================
// SPHERE MESH DRAWING
// =========================================================================================
void SphereMesh::Draw(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& worldViewProjection,
    const DirectX::XMMATRIX& world,
    const DirectX::XMFLOAT3& cameraPosition
)
{
    if (context == nullptr ||
        !inputLayout ||
        !vertexShader ||
        !vertexBuffer ||
        !indexBuffer ||
        !transformationBuffer ||
        indexCount == 0u ||
        !IsFinitePosition(cameraPosition) ||
        DirectX::XMMatrixIsNaN(worldViewProjection) ||
        DirectX::XMMatrixIsInfinite(worldViewProjection) ||
        DirectX::XMMatrixIsNaN(world) ||
        DirectX::XMMatrixIsInfinite(world))
    {
        return;
    }

    const DirectX::XMVECTOR determinant =
        DirectX::XMMatrixDeterminant(world);

    const float determinantValue =
        DirectX::XMVectorGetX(determinant);

    if (!std::isfinite(determinantValue) ||
        std::abs(determinantValue) <=
            std::numeric_limits<float>::epsilon())
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    const HRESULT mapResult =
        context->Map(
            transformationBuffer.Get(),
            0u,
            D3D11_MAP_WRITE_DISCARD,
            0u,
            &mappedResource
        );

    if (FAILED(mapResult) ||
        mappedResource.pData == nullptr)
    {
        return;
    }

    TransformationBuffer transformations = {};
    transformations.worldViewProjection =
        DirectX::XMMatrixTranspose(
            worldViewProjection
        );

    transformations.world =
        DirectX::XMMatrixTranspose(
            world
        );

    transformations.normal =
        DirectX::XMMatrixTranspose(
            DirectX::XMMatrixInverse(
                nullptr,
                world
            )
        );

    transformations.cameraPosition = cameraPosition;

    *static_cast<TransformationBuffer*>(
        mappedResource.pData
    ) = transformations;

    context->Unmap(
        transformationBuffer.Get(),
        0u
    );

    ID3D11Buffer* constantBuffers[] =
    {
        transformationBuffer.Get()
    };

    context->VSSetConstantBuffers(
        0u,
        1u,
        constantBuffers
    );

    context->PSSetConstantBuffers(
        0u,
        1u,
        constantBuffers
    );

    context->IASetInputLayout(
        inputLayout.Get()
    );

    context->VSSetShader(
        vertexShader.Get(),
        nullptr,
        0u
    );

    ID3D11Buffer* vertexBuffers[] =
    {
        vertexBuffer.Get()
    };

    const UINT stride = sizeof(SphereVertex);
    const UINT offset = 0u;

    context->IASetVertexBuffers(
        0u,
        1u,
        vertexBuffers,
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        indexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0u
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->DrawIndexed(
        indexCount,
        0u,
        0
    );
}