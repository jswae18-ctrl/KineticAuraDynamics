#include "HudImageRenderer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace
{
    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "HudImageRenderer requires a valid Direct3D device."
            );
        }

        return device;
    }

    HudImagePipeline& ValidatePipeline(
        HudImagePipeline& pipeline
    ) noexcept
    {
        return pipeline;
    }

    ID3D11ShaderResourceView* ValidateTextureView(
        ID3D11ShaderResourceView* textureView
    )
    {
        if (textureView == nullptr)
        {
            throw std::invalid_argument(
                "HudImageRenderer requires a valid texture view."
            );
        }

        return textureView;
    }

    DirectX::XMFLOAT4 ValidateTintColor(
        const DirectX::XMFLOAT4& color
    )
    {
        if (!std::isfinite(color.x) ||
            !std::isfinite(color.y) ||
            !std::isfinite(color.z) ||
            !std::isfinite(color.w))
        {
            throw std::invalid_argument(
                "HudImageRenderer requires finite tint components."
            );
        }

        if (color.x < 0.0f || color.x > 1.0f ||
            color.y < 0.0f || color.y > 1.0f ||
            color.z < 0.0f || color.z > 1.0f ||
            color.w < 0.0f || color.w > 1.0f)
        {
            throw std::out_of_range(
                "HudImageRenderer tint components must be between zero and one."
            );
        }

        return color;
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
}

// =========================================================================================
// HUD IMAGE RENDERER CONSTRUCTION
// =========================================================================================
HudImageRenderer::HudImageRenderer(
    ID3D11Device* device,
    HudImagePipeline& pipeline,
    ID3D11ShaderResourceView* textureView,
    const DirectX::XMFLOAT4& tintColor
)
    :
    pipeline(ValidatePipeline(pipeline)),
    textureView(ValidateTextureView(textureView)),
    tintColor(ValidateTintColor(tintColor))
{
    device = ValidateDevice(device);

    // =====================================================================================
    // HUD IMAGE VERTEX BUFFER
    // =====================================================================================
    D3D11_BUFFER_DESC vertexBufferDescription = {};
    vertexBufferDescription.ByteWidth = sizeof(HudVertex) * 4u;
    vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(
        device->CreateBuffer(
            &vertexBufferDescription,
            nullptr,
            vertexBuffer.GetAddressOf()
        ),
        "Failed to create the HUD image vertex buffer."
    );

    // =====================================================================================
    // HUD IMAGE COLOR BUFFER
    // =====================================================================================
    static_assert(
        sizeof(HudColorBuffer) % 16u == 0u,
        "HUD image color-buffer size must be aligned to 16 bytes."
    );

    D3D11_BUFFER_DESC colorBufferDescription = {};
    colorBufferDescription.ByteWidth = sizeof(HudColorBuffer);
    colorBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
    colorBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    colorBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(
        device->CreateBuffer(
            &colorBufferDescription,
            nullptr,
            colorBuffer.GetAddressOf()
        ),
        "Failed to create the HUD image color buffer."
    );
}

// =========================================================================================
// HUD IMAGE TINT
// =========================================================================================
void HudImageRenderer::SetTint(
    const DirectX::XMFLOAT4& color
) noexcept
{
    if (!std::isfinite(color.x) ||
        !std::isfinite(color.y) ||
        !std::isfinite(color.z) ||
        !std::isfinite(color.w))
    {
        return;
    }

    tintColor.x = (std::max)(0.0f, (std::min)(color.x, 1.0f));
    tintColor.y = (std::max)(0.0f, (std::min)(color.y, 1.0f));
    tintColor.z = (std::max)(0.0f, (std::min)(color.z, 1.0f));
    tintColor.w = (std::max)(0.0f, (std::min)(color.w, 1.0f));
}

// =========================================================================================
// HUD IMAGE VERTEX UPDATE
// =========================================================================================
void HudImageRenderer::UpdateVertices(
    ID3D11DeviceContext* context,
    float left,
    float top,
    float width,
    float height,
    unsigned int screenWidth,
    unsigned int screenHeight
)
{
    const float screenWidthFloat = static_cast<float>(screenWidth);
    const float screenHeightFloat = static_cast<float>(screenHeight);

    const float right = left + width;
    const float bottom = top + height;

    // Convert screen pixels into normalized device coordinates.
    const float leftNdc =
        (left / screenWidthFloat) * 2.0f - 1.0f;

    const float rightNdc =
        (right / screenWidthFloat) * 2.0f - 1.0f;

    const float topNdc =
        1.0f - (top / screenHeightFloat) * 2.0f;

    const float bottomNdc =
        1.0f - (bottom / screenHeightFloat) * 2.0f;

    const HudVertex vertices[] =
    {
        {{leftNdc, topNdc}, {0.0f, 0.0f}},
        {{rightNdc, topNdc}, {1.0f, 0.0f}},
        {{rightNdc, bottomNdc}, {1.0f, 1.0f}},
        {{leftNdc, bottomNdc}, {0.0f, 1.0f}}
    };

    D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer = {};

    ThrowIfFailed(
        context->Map(
            vertexBuffer.Get(),
            0u,
            D3D11_MAP_WRITE_DISCARD,
            0u,
            &mappedVertexBuffer
        ),
        "Failed to map the HUD image vertex buffer."
    );

    std::memcpy(
        mappedVertexBuffer.pData,
        vertices,
        sizeof(vertices)
    );

    context->Unmap(
        vertexBuffer.Get(),
        0u
    );
}

// =========================================================================================
// HUD IMAGE COLOR UPDATE
// =========================================================================================
void HudImageRenderer::UpdateColorBuffer(
    ID3D11DeviceContext* context
)
{
    D3D11_MAPPED_SUBRESOURCE mappedColorBuffer = {};

    ThrowIfFailed(
        context->Map(
            colorBuffer.Get(),
            0u,
            D3D11_MAP_WRITE_DISCARD,
            0u,
            &mappedColorBuffer
        ),
        "Failed to map the HUD image color buffer."
    );

    const HudColorBuffer colorData =
    {
        tintColor
    };

    std::memcpy(
        mappedColorBuffer.pData,
        &colorData,
        sizeof(colorData)
    );

    context->Unmap(
        colorBuffer.Get(),
        0u
    );
}

// =========================================================================================
// HUD IMAGE DRAWING
// =========================================================================================
void HudImageRenderer::Draw(
    ID3D11DeviceContext* context,
    float left,
    float top,
    float width,
    float height,
    unsigned int screenWidth,
    unsigned int screenHeight
)
{
    if (context == nullptr ||
        screenWidth == 0u ||
        screenHeight == 0u)
    {
        return;
    }

    if (!std::isfinite(left) ||
        !std::isfinite(top) ||
        !std::isfinite(width) ||
        !std::isfinite(height) ||
        width <= 0.0f ||
        height <= 0.0f)
    {
        return;
    }

    UpdateVertices(
        context,
        left,
        top,
        width,
        height,
        screenWidth,
        screenHeight
    );

    UpdateColorBuffer(context);

    pipeline.Bind(context);

    ID3D11Buffer* vertexBuffers[] =
    {
        vertexBuffer.Get()
    };

    const UINT stride = sizeof(HudVertex);
    const UINT offset = 0u;

    context->IASetVertexBuffers(
        0u,
        1u,
        vertexBuffers,
        &stride,
        &offset
    );

    ID3D11ShaderResourceView* textureViews[] =
    {
        textureView.Get()
    };

    context->PSSetShaderResources(
        0u,
        1u,
        textureViews
    );

    ID3D11Buffer* colorBuffers[] =
    {
        colorBuffer.Get()
    };

    // HudColorBuffer is bound to pixel-shader slot b0.
    context->PSSetConstantBuffers(
        0u,
        1u,
        colorBuffers
    );

    context->DrawIndexed(
        6u,
        0u,
        0
    );
}