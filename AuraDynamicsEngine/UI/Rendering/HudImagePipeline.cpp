#include "HudImagePipeline.h"
#include <iterator>
#include <stdexcept>
#include <string>

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
    constexpr const wchar_t* HudImageVertexShaderPath =
        L"Shaders\\HudImageVertexShader.hlsl";

    constexpr const wchar_t* HudImagePixelShaderPath =
        L"Shaders\\HudImagePixelShader.hlsl";

    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "HudImagePipeline requires a valid Direct3D device."
            );
        }

        return device;
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

    void ReportShaderCompilerError(
        ID3DBlob* errorBlob
    ) noexcept
    {
        if (errorBlob == nullptr ||
            errorBlob->GetBufferPointer() == nullptr ||
            errorBlob->GetBufferSize() == 0u)
        {
            return;
        }

        try
        {
            const std::string compilerError(
                static_cast<const char*>(errorBlob->GetBufferPointer()),
                errorBlob->GetBufferSize()
            );

            OutputDebugStringA(compilerError.c_str());
        }
        catch (...)
        {
            OutputDebugStringA(
                "HudImagePipeline could not format the shader compiler error.\n"
            );
        }
    }

    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const wchar_t* shaderPath,
        const char* shaderTarget,
        const char* failureMessage
    )
    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG;
#endif

        const HRESULT result =
            D3DCompileFromFile(
                shaderPath,
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main",
                shaderTarget,
                compileFlags,
                0u,
                shaderBlob.GetAddressOf(),
                errorBlob.GetAddressOf()
            );

        if (FAILED(result))
        {
            ReportShaderCompilerError(errorBlob.Get());
            throw std::runtime_error(failureMessage);
        }

        if (!shaderBlob)
        {
            throw std::runtime_error(
                "HudImagePipeline received an empty compiled shader."
            );
        }

        return shaderBlob;
    }
}

// =========================================================================================
// HUD IMAGE PIPELINE CONSTRUCTION
// =========================================================================================
HudImagePipeline::HudImagePipeline(ID3D11Device* device)
{
    device = ValidateDevice(device);

    // =====================================================================================
    // HUD IMAGE SHADERS
    // =====================================================================================
    const Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob =
        CompileShader(
            HudImageVertexShaderPath,
            "vs_5_0",
            "Failed to compile the HUD image vertex shader."
        );

    ThrowIfFailed(
        device->CreateVertexShader(
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            nullptr,
            vertexShader.GetAddressOf()
        ),
        "Failed to create the HUD image vertex shader."
    );

    const D3D11_INPUT_ELEMENT_DESC inputElements[] =
    {
        {
            "POSITION",
            0u,
            DXGI_FORMAT_R32G32_FLOAT,
            0u,
            0u,
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        },
        {
            "TEXCOORD",
            0u,
            DXGI_FORMAT_R32G32_FLOAT,
            0u,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        }
    };

    ThrowIfFailed(
        device->CreateInputLayout(
            inputElements,
            static_cast<UINT>(std::size(inputElements)),
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            inputLayout.GetAddressOf()
        ),
        "Failed to create the HUD image input layout."
    );

    const Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob =
        CompileShader(
            HudImagePixelShaderPath,
            "ps_5_0",
            "Failed to compile the HUD image pixel shader."
        );

    ThrowIfFailed(
        device->CreatePixelShader(
            pixelShaderBlob->GetBufferPointer(),
            pixelShaderBlob->GetBufferSize(),
            nullptr,
            pixelShader.GetAddressOf()
        ),
        "Failed to create the HUD image pixel shader."
    );

    // =====================================================================================
    // HUD IMAGE INDEX BUFFER
    // =====================================================================================
    const unsigned int indices[] =
    {
        0u, 1u, 2u,
        0u, 2u, 3u
    };

    D3D11_BUFFER_DESC indexBufferDescription = {};
    indexBufferDescription.ByteWidth = sizeof(indices);
    indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    ThrowIfFailed(
        device->CreateBuffer(
            &indexBufferDescription,
            &indexData,
            indexBuffer.GetAddressOf()
        ),
        "Failed to create the HUD image index buffer."
    );

    // =====================================================================================
    // HUD IMAGE SAMPLER
    // =====================================================================================
    D3D11_SAMPLER_DESC samplerDescription = {};
    samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

    ThrowIfFailed(
        device->CreateSamplerState(
            &samplerDescription,
            samplerState.GetAddressOf()
        ),
        "Failed to create the HUD image sampler state."
    );

    // =====================================================================================
    // HUD IMAGE BLEND STATE
    // =====================================================================================
    D3D11_BLEND_DESC blendDescription = {};
    blendDescription.AlphaToCoverageEnable = false;
    blendDescription.IndependentBlendEnable = false;

    D3D11_RENDER_TARGET_BLEND_DESC& renderTargetBlend =
        blendDescription.RenderTarget[0];

    renderTargetBlend.BlendEnable = true;
    renderTargetBlend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    renderTargetBlend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    renderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
    renderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
    renderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
    renderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    renderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(
        device->CreateBlendState(
            &blendDescription,
            blendState.GetAddressOf()
        ),
        "Failed to create the HUD image blend state."
    );

    // =====================================================================================
    // HUD IMAGE DEPTH STATE
    // =====================================================================================
    D3D11_DEPTH_STENCIL_DESC depthDescription = {};
    depthDescription.DepthEnable = false;
    depthDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthDescription.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthDescription.StencilEnable = false;

    ThrowIfFailed(
        device->CreateDepthStencilState(
            &depthDescription,
            depthDisabledState.GetAddressOf()
        ),
        "Failed to create the HUD image depth-disabled state."
    );
}

// =========================================================================================
// HUD IMAGE PIPELINE BINDING
// =========================================================================================
void HudImagePipeline::Bind(
    ID3D11DeviceContext* context
) const noexcept
{
    if (context == nullptr)
    {
        return;
    }

    context->IASetInputLayout(inputLayout.Get());

    context->IASetIndexBuffer(
        indexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0u
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->VSSetShader(
        vertexShader.Get(),
        nullptr,
        0u
    );

    context->PSSetShader(
        pixelShader.Get(),
        nullptr,
        0u
    );

    ID3D11SamplerState* sampler = samplerState.Get();

    context->PSSetSamplers(
        0u,
        1u,
        &sampler
    );

    const float blendFactor[4] =
    {
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    context->OMSetBlendState(
        blendState.Get(),
        blendFactor,
        0xFFFFFFFFu
    );

    context->OMSetDepthStencilState(
        depthDisabledState.Get(),
        0u
    );
}