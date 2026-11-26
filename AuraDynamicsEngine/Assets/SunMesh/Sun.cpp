#include "Sun.h"
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
                "Sun requires a valid Direct3D device."
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
                "Sun requires a valid Direct3D device context."
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

    // =====================================================================================
    // SUN PIXEL SHADER
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11PixelShader> CreateSunPixelShader(
        ID3D11Device* device
    )
    {
        const std::wstring shaderPath =
            L"Shaders\\SunPixelShader.hlsl";

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
                "Failed to compile the Sun pixel shader."
            );
        }

        if (!shaderBytecode ||
            shaderBytecode->GetBufferPointer() == nullptr ||
            shaderBytecode->GetBufferSize() == 0u)
        {
            throw std::runtime_error(
                "Sun pixel shader compilation returned no bytecode."
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
            "Failed to create the Sun pixel shader."
        );

        return pixelShader;
    }
}

// =========================================================================================
// SUN CONSTRUCTION
// =========================================================================================
Sun::Sun(
    ID3D11Device* device,
    ID3D11DeviceContext* context
)
    :
    sphereMesh(
        ValidateDevice(device)
    ),
    sunTexture(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Sun\\sun.jpg"
    ),
    sunPixelShader(
        CreateSunPixelShader(
            ValidateDevice(device)
        )
    )
{
}

// =========================================================================================
// SUN DRAWING
// =========================================================================================
void Sun::Draw(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& worldViewProjection,
    const DirectX::XMMATRIX& world,
    const DirectX::XMFLOAT3& cameraPosition
)
{
    if (context == nullptr ||
        !sunPixelShader)
    {
        return;
    }

    sunTexture.Bind(
        context,
        0u
    );

    context->PSSetShader(
        sunPixelShader.Get(),
        nullptr,
        0u
    );

    sphereMesh.Draw(
        context,
        worldViewProjection,
        world,
        cameraPosition
    );
}