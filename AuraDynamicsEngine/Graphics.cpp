#include "Graphics.h"
#include <stdexcept>
#include <utility>

namespace
{
    void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
        {
            throw std::runtime_error(message);
        }
    }
    
    unsigned int ValidateGraphicsDimension(unsigned int dimension)
    {
        if (dimension == 0u)
        {
            throw std::invalid_argument(
                "Graphics dimensions must be greater than zero."
            );
        }

        return dimension;
    }

    HWND ValidateWindowHandle(HWND windowHandle)
    {
        if (!windowHandle || !IsWindow(windowHandle))
        {
            throw std::invalid_argument(
                "Graphics requires a valid window handle."
            );
        }

        return windowHandle;
    }
}

Graphics::Graphics(HWND windowHandle, unsigned int width, unsigned int height)
    :
    width(ValidateGraphicsDimension(width)),
    height(ValidateGraphicsDimension(height)),
    windowHandle(ValidateWindowHandle(windowHandle))
{
    // =====================================================================================
    // DIRECT3D DEVICE CREATION
    // =====================================================================================
    UINT deviceFlags = 0;

#ifdef _DEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    

    ThrowIfFailed(
        D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            deviceFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),
            nullptr,
            context.GetAddressOf()
        ),
        "Failed to create the Direct3D 11 hardware device."
    );

    // =====================================================================================
    // SWAP CHAIN CONFIGURATION
    // =====================================================================================
    DXGI_SWAP_CHAIN_DESC swapChainDescription = {};

    swapChainDescription.BufferDesc.Width = width;
    swapChainDescription.BufferDesc.Height = height;
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDescription.BufferDesc.ScanlineOrdering =
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.SampleDesc.Quality = 0;

    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.BufferCount = 1;
    swapChainDescription.OutputWindow = windowHandle;
    swapChainDescription.Windowed = TRUE;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescription.Flags = 0;

    // =====================================================================================
    // DXGI FACTORY RESOLUTION
    // =====================================================================================

    /* Resolve the DXGI factory through the device's adapter before creating
     * the swap chain for this window. */
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;

    ThrowIfFailed(
        device.As(&dxgiDevice),
        "Failed to acquire the DXGI device interface."
    );

    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;

    ThrowIfFailed(
        dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()),
        "Failed to acquire the DXGI adapter."
    );

    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

    ThrowIfFailed(
        dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())),
        "Failed to acquire the DXGI factory."
    );

    // =====================================================================================
    // SWAP CHAIN CREATION
    // =====================================================================================
    ThrowIfFailed(
        dxgiFactory->CreateSwapChain(
            device.Get(),
            &swapChainDescription,
            swapChain.GetAddressOf()
        ),
        "Failed to create the DXGI swap chain."
    );

    // Create the initial render target, depth buffer, and viewport.
    Resize(width, height);

    // =====================================================================================
    // DEPTH-STENCIL STATE
    // =====================================================================================
    D3D11_DEPTH_STENCIL_DESC depthStencilDescription = {};

    depthStencilDescription.DepthEnable = TRUE;
    depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;

    ThrowIfFailed(
        device->CreateDepthStencilState(
            &depthStencilDescription,
            depthStencilState.GetAddressOf()
        ),
        "Failed to create the depth-stencil state."
    );

    // =====================================================================================
    // RASTERIZER STATE
    // =====================================================================================
    D3D11_RASTERIZER_DESC rasterizerDescription = {};

    rasterizerDescription.FillMode = D3D11_FILL_SOLID;
    rasterizerDescription.CullMode = D3D11_CULL_BACK;
    rasterizerDescription.DepthClipEnable = TRUE;

    ThrowIfFailed(
        device->CreateRasterizerState(
            &rasterizerDescription,
            rasterizerState.GetAddressOf()
        ),
        "Failed to create the rasterizer state."
    );
}

void Graphics::Resize(unsigned int newWidth, unsigned int newHeight)
{
    if (newWidth == 0 || newHeight == 0)
    {
        return;
    }

    // =====================================================================================
    // RELEASE WINDOW-SIZE RESOURCES
    // =====================================================================================

    /* ResizeBuffers requires every direct and indirect reference to the old
     * back buffer to be released before the swap chain can be resized. */
    context->OMSetRenderTargets(0, nullptr, nullptr);

    renderTargetView.Reset();
    depthStencilView.Reset();

    // =====================================================================================
    // SWAP CHAIN RESIZE
    // =====================================================================================
    ThrowIfFailed(
        swapChain->ResizeBuffers(
            0u,
            newWidth,
            newHeight,
            DXGI_FORMAT_UNKNOWN,
            0u
        ),
        "Failed to resize the DXGI swap-chain buffers."
    );

    // =====================================================================================
    // BACK-BUFFER RESOURCES
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    ThrowIfFailed(
        swapChain->GetBuffer(
            0u,
            IID_PPV_ARGS(backBuffer.GetAddressOf())
        ),
        "Failed to acquire the resized swap-chain back buffer."
    );

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> newRenderTargetView;

    ThrowIfFailed(
        device->CreateRenderTargetView(
            backBuffer.Get(),
            nullptr,
            newRenderTargetView.GetAddressOf()
        ),
        "Failed to create the resized render-target view."
    );

    // =====================================================================================
    // DEPTH-BUFFER RESOURCES
    // =====================================================================================
    D3D11_TEXTURE2D_DESC depthTextureDescription = {};

    depthTextureDescription.Width = newWidth;
    depthTextureDescription.Height = newHeight;
    depthTextureDescription.MipLevels = 1;
    depthTextureDescription.ArraySize = 1;
    depthTextureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthTextureDescription.SampleDesc.Count = 1;
    depthTextureDescription.SampleDesc.Quality = 0;
    depthTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;

    ThrowIfFailed(
        device->CreateTexture2D(
            &depthTextureDescription,
            nullptr,
            depthStencilTexture.GetAddressOf()
        ),
        "Failed to create the resized depth-stencil texture."
    );

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> newDepthStencilView;

    ThrowIfFailed(
        device->CreateDepthStencilView(
            depthStencilTexture.Get(),
            nullptr,
            newDepthStencilView.GetAddressOf()
        ),
        "Failed to create the resized depth-stencil view."
    );
    
    // Store the replacement views only after both were created successfully.
    renderTargetView = std::move(newRenderTargetView);
    depthStencilView = std::move(newDepthStencilView);

    width = newWidth;
    height = newHeight;

    // =====================================================================================
    // VIEWPORT
    // =====================================================================================

    // Map normalized device coordinates to the current client dimensions.
    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1u, &viewport);
}

void Graphics::ClearBuffers(float r, float g, float b) noexcept
{
    // =====================================================================================
    // OUTPUT-MERGER STATE
    // =====================================================================================
    context->OMSetRenderTargets(
        1u,
        renderTargetView.GetAddressOf(),
        depthStencilView.Get()
    );

    context->OMSetDepthStencilState(
        depthStencilState.Get(),
        1u
    );

    context->RSSetState(rasterizerState.Get());

    // =====================================================================================
    // FRAME CLEAR
    // =====================================================================================
    const float clearColor[] =
    {
        r,
        g,
        b,
        1.0f
    };

    context->ClearRenderTargetView(
        renderTargetView.Get(),
        clearColor
    );

    context->ClearDepthStencilView(
        depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0u
    );
}

void Graphics::Present()
{
    ThrowIfFailed(
        swapChain->Present(1u, 0u),
        "Failed to present the completed frame."
    );
}