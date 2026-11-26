#include "Space.h"
#include <d3dcompiler.h>
#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
    // =====================================================================================
    // DIRECT3D DEPENDENCY VALIDATION
    // =====================================================================================
    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "Space requires a valid Direct3D device."
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
                "Space requires a valid Direct3D device context."
            );
        }

        return context;
    }

    void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
        {
            throw std::runtime_error(message);
        }
    }

    // =====================================================================================
    // SPACE PIXEL SHADER
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11PixelShader> CreateSpacePixelShader(
        ID3D11Device* device
    )
    {
        const std::wstring shaderPath =
            L"Shaders\\SpacePixelShader.hlsl";

        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
        compileFlags |=
            D3DCOMPILE_DEBUG |
            D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
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
                pixelShaderBlob.GetAddressOf(),
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
                "Failed to compile the Space pixel shader."
            );
        }

        if (!pixelShaderBlob ||
            pixelShaderBlob->GetBufferPointer() == nullptr ||
            pixelShaderBlob->GetBufferSize() == 0u)
        {
            throw std::runtime_error(
                "Space pixel shader compilation returned no bytecode."
            );
        }

        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

        ThrowIfFailed(
            device->CreatePixelShader(
                pixelShaderBlob->GetBufferPointer(),
                pixelShaderBlob->GetBufferSize(),
                nullptr,
                pixelShader.GetAddressOf()
            ),
            "Failed to create the Space pixel shader."
        );

        return pixelShader;
    }

    // =====================================================================================
    // INSIDE-SPHERE RASTERIZER STATE
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> CreateInsideRasterizerState(
        ID3D11Device* device
    )
    {
        D3D11_RASTERIZER_DESC description = {};
        description.FillMode = D3D11_FILL_SOLID;
        description.CullMode = D3D11_CULL_FRONT;
        description.FrontCounterClockwise = FALSE;
        description.DepthClipEnable = TRUE;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

        ThrowIfFailed(
            device->CreateRasterizerState(
                &description,
                rasterizerState.GetAddressOf()
            ),
            "Failed to create the Space rasterizer state."
        );

        return rasterizerState;
    }

    // =====================================================================================
    // BACKGROUND DEPTH STATE
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateBackgroundDepthState(
        ID3D11Device* device
    )
    {
        D3D11_DEPTH_STENCIL_DESC description = {};
        description.DepthEnable = FALSE;
        description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        description.DepthFunc = D3D11_COMPARISON_ALWAYS;
        description.StencilEnable = FALSE;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;

        ThrowIfFailed(
            device->CreateDepthStencilState(
                &description,
                depthState.GetAddressOf()
            ),
            "Failed to create the Space depth-stencil state."
        );

        return depthState;
    }

    // =====================================================================================
    // PIPELINE STATE RESTORATION
    // =====================================================================================
    class ScopedSpacePipelineState final
    {
    public:
        explicit ScopedSpacePipelineState(ID3D11DeviceContext* context) noexcept
            :
            context(context)
        {
            if (this->context == nullptr)
            {
                return;
            }

            this->context->RSGetState(
                previousRasterizerState.GetAddressOf()
            );

            this->context->OMGetDepthStencilState(
                previousDepthState.GetAddressOf(),
                &previousStencilReference
            );

            this->context->PSGetShader(
                previousPixelShader.GetAddressOf(),
                nullptr,
                nullptr
            );

            this->context->PSGetShaderResources(
                0u,
                1u,
                previousShaderResource0.GetAddressOf()
            );

            this->context->PSGetShaderResources(
                1u,
                1u,
                previousShaderResource1.GetAddressOf()
            );

            this->context->PSGetSamplers(
                0u,
                1u,
                previousSampler0.GetAddressOf()
            );

            this->context->PSGetSamplers(
                1u,
                1u,
                previousSampler1.GetAddressOf()
            );
        }

        ~ScopedSpacePipelineState()
        {
            if (context == nullptr)
            {
                return;
            }

            context->RSSetState(
                previousRasterizerState.Get()
            );

            context->OMSetDepthStencilState(
                previousDepthState.Get(),
                previousStencilReference
            );

            context->PSSetShader(
                previousPixelShader.Get(),
                nullptr,
                0u
            );

            ID3D11ShaderResourceView* shaderResources[] =
            {
                previousShaderResource0.Get(),
                previousShaderResource1.Get()
            };

            context->PSSetShaderResources(
                0u,
                2u,
                shaderResources
            );

            ID3D11SamplerState* samplers[] =
            {
                previousSampler0.Get(),
                previousSampler1.Get()
            };

            context->PSSetSamplers(
                0u,
                2u,
                samplers
            );
        }

        ScopedSpacePipelineState(const ScopedSpacePipelineState&) = delete;
        ScopedSpacePipelineState& operator=(const ScopedSpacePipelineState&) = delete;
        ScopedSpacePipelineState(ScopedSpacePipelineState&&) = delete;
        ScopedSpacePipelineState& operator=(ScopedSpacePipelineState&&) = delete;

    private:
        ID3D11DeviceContext* context = nullptr;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState>
            previousRasterizerState;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState>
            previousDepthState;

        Microsoft::WRL::ComPtr<ID3D11PixelShader>
            previousPixelShader;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
            previousShaderResource0;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
            previousShaderResource1;

        Microsoft::WRL::ComPtr<ID3D11SamplerState>
            previousSampler0;

        Microsoft::WRL::ComPtr<ID3D11SamplerState>
            previousSampler1;

        UINT previousStencilReference = 0u;
    };

    bool IsFinitePosition(
        const DirectX::XMFLOAT3& position
    ) noexcept
    {
        return
            std::isfinite(position.x) &&
            std::isfinite(position.y) &&
            std::isfinite(position.z);
    }
}

// =========================================================================================
// SPACE CONSTRUCTION
// =========================================================================================
Space::Space(
    ID3D11Device* device,
    ID3D11DeviceContext* context
)
    :
    spaceSphere(
        ValidateDevice(device)
    ),
    starTexture(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Space\\Stars.jpg"
    ),
    milkyWayTexture(
        ValidateDevice(device),
        ValidateDeviceContext(context),
        L"Textures\\Space\\MilkyWayStars.jpg"
    ),
    spacePixelShader(
        CreateSpacePixelShader(
            ValidateDevice(device)
        )
    ),
    insideRasterizerState(
        CreateInsideRasterizerState(
            ValidateDevice(device)
        )
    ),
    backgroundDepthState(
        CreateBackgroundDepthState(
            ValidateDevice(device)
        )
    )
{
}

// =========================================================================================
// SPACE DRAWING
// =========================================================================================
void Space::Draw(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection,
    const DirectX::XMFLOAT3& cameraPosition
)
{
    if (context == nullptr ||
        !spacePixelShader ||
        !insideRasterizerState ||
        !backgroundDepthState ||
        !IsFinitePosition(cameraPosition))
    {
        return;
    }

    const ScopedSpacePipelineState previousPipelineState(context);

    const DirectX::XMMATRIX spaceWorld =
        DirectX::XMMatrixScaling(
            SpaceSphereScale,
            SpaceSphereScale,
            SpaceSphereScale
        ) *
        DirectX::XMMatrixTranslation(
            cameraPosition.x,
            cameraPosition.y,
            cameraPosition.z
        );

    const DirectX::XMMATRIX spaceWorldViewProjection =
        spaceWorld *
        view *
        projection;

    context->RSSetState(
        insideRasterizerState.Get()
    );

    context->OMSetDepthStencilState(
        backgroundDepthState.Get(),
        0u
    );

    starTexture.Bind(context, 0u);
    milkyWayTexture.Bind(context, 1u);

    context->PSSetShader(
        spacePixelShader.Get(),
        nullptr,
        0u
    );

    spaceSphere.Draw(
        context,
        spaceWorldViewProjection,
        spaceWorld,
        cameraPosition
    );
}