#include "Earth.h"

#include <cmath>
#include <stdexcept>
#include <string>

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
                "Earth requires a valid Direct3D device."
            );
        }

        return device;
    }

    ID3D11DeviceContext* ValidateDeviceContext(
        ID3D11DeviceContext* context
    )
    {
        if (context == nullptr)
        {
            throw std::invalid_argument(
                "Earth requires a valid Direct3D device context."
            );
        }

        return context;
    }

    void ThrowIfFailed(
        HRESULT result,
        const char* message
    )
    {
        if (FAILED(result))
        {
            throw std::runtime_error(message);
        }
    }

    bool IsFinitePosition(
        const DirectX::XMFLOAT3& position
    ) noexcept
    {
        return
            std::isfinite(position.x) &&
            std::isfinite(position.y) &&
            std::isfinite(position.z);
    }

    // =====================================================================================
    // EARTH PIXEL SHADER
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11PixelShader> CreateEarthPixelShader(
        ID3D11Device* device
    )
    {
        const std::wstring shaderPath =
            L"Shaders\\EarthPixelShader.hlsl";

        UINT compileFlags =
            D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
        compileFlags |=
            D3DCOMPILE_DEBUG |
            D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBytecode;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        const HRESULT compileResult =
            D3DCompileFromFile(
                shaderPath.c_str(),
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main",
                "ps_5_0",
                compileFlags,
                0u,
                shaderBytecode.GetAddressOf(),
                errorBlob.GetAddressOf()
            );

        if (FAILED(compileResult))
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
                "Failed to compile the Earth pixel shader."
            );
        }

        if (!shaderBytecode ||
            shaderBytecode->GetBufferPointer() == nullptr ||
            shaderBytecode->GetBufferSize() == 0u)
        {
            throw std::runtime_error(
                "Earth pixel shader compilation returned no bytecode."
            );
        }

        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

        ThrowIfFailed(
            device->CreatePixelShader(
                shaderBytecode->GetBufferPointer(),
                shaderBytecode->GetBufferSize(),
                nullptr,
                pixelShader.GetAddressOf()
            ),
            "Failed to create the Earth pixel shader."
        );

        return pixelShader;
    }

    // =====================================================================================
    // EARTH LIGHTING BUFFER
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateEarthLightingBuffer(
        ID3D11Device* device
    )
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth = sizeof(Earth::EarthLightingBuffer);
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
            "Failed to create the Earth lighting buffer."
        );

        return buffer;
    }
}

// =========================================================================================
// EARTH CONSTRUCTION
// =========================================================================================
Earth::Earth(
    ID3D11Device* device,
    ID3D11DeviceContext* context
)
    :
    sphere(
        ValidateDevice(device)
    ),
    dayMap(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Earth\\EarthDayMapping.jpg"
    ),
    specularMap(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Earth\\EarthSpecularMapping.tif"
    ),
    nightMap(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Earth\\EarthNightMapping.jpg"
    ),
    normalMap(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Earth\\EarthNormalMapping.tif"
    ),
    cloudMap(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Earth\\EarthClouds.jpg"
    ),
    earthPixelShader(
        CreateEarthPixelShader(
            ValidateDevice(device)
        )
    ),
    earthLightingBuffer(
        CreateEarthLightingBuffer(
            ValidateDevice(device)
        )
    )
{
}

// =========================================================================================
// EARTH UPDATE
// =========================================================================================
void Earth::Update(float deltaTime) noexcept
{
    if (!std::isfinite(deltaTime) ||
        deltaTime <= 0.0f)
    {
        return;
    }

    cloudOffset +=
        CloudMovementSpeed *
        deltaTime;

    if (!std::isfinite(cloudOffset))
    {
        cloudOffset = 0.0f;
        return;
    }

    cloudOffset =
        std::fmod(
            cloudOffset,
            1.0f
        );

    if (cloudOffset < 0.0f)
    {
        cloudOffset += 1.0f;
    }
}

// =========================================================================================
// EARTH DRAWING
// =========================================================================================
void Earth::Draw(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& worldViewProjection,
    const DirectX::XMMATRIX& world,
    const DirectX::XMFLOAT3& cameraPosition,
    const DirectX::XMFLOAT3& sunPosition
)
{
    if (context == nullptr ||
        !earthPixelShader ||
        !earthLightingBuffer ||
        !IsFinitePosition(cameraPosition) ||
        !IsFinitePosition(sunPosition) ||
        DirectX::XMMatrixIsNaN(worldViewProjection) ||
        DirectX::XMMatrixIsInfinite(worldViewProjection) ||
        DirectX::XMMatrixIsNaN(world) ||
        DirectX::XMMatrixIsInfinite(world))
    {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mappedLighting = {};

    ThrowIfFailed(
        context->Map(
            earthLightingBuffer.Get(),
            0u,
            D3D11_MAP_WRITE_DISCARD,
            0u,
            &mappedLighting
        ),
        "Failed to map the Earth lighting buffer."
    );

    if (mappedLighting.pData == nullptr)
    {
        context->Unmap(
            earthLightingBuffer.Get(),
            0u
        );

        throw std::runtime_error(
            "Earth lighting buffer mapping returned no writable memory."
        );
    }

    EarthLightingBuffer lightingData = {};
    lightingData.sunPosition = sunPosition;
    lightingData.cloudOffset = cloudOffset;

    *static_cast<EarthLightingBuffer*>(
        mappedLighting.pData
    ) = lightingData;

    context->Unmap(
        earthLightingBuffer.Get(),
        0u
    );

    ID3D11Buffer* lightingBuffers[] =
    {
        earthLightingBuffer.Get()
    };

    context->PSSetConstantBuffers(
        1u,
        1u,
        lightingBuffers
    );

    dayMap.Bind(
        context,
        0u
    );

    specularMap.Bind(
        context,
        1u
    );

    nightMap.Bind(
        context,
        2u
    );

    normalMap.Bind(
        context,
        3u
    );

    cloudMap.Bind(
        context,
        4u
    );

    context->PSSetShader(
        earthPixelShader.Get(),
        nullptr,
        0u
    );

    sphere.Draw(
        context,
        worldViewProjection,
        world,
        cameraPosition
    );
}