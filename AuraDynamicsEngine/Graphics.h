#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
class Graphics
{
public:
    Graphics(HWND windowHandle, unsigned int width, unsigned int height);
    ~Graphics() = default;

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;

    void ClearBuffers(float r, float g, float b) noexcept;
    void Resize(unsigned int newWidth, unsigned int newHeight);
    void Present();

    ID3D11Device* GetDevice() const noexcept { return device.Get(); }
    ID3D11DeviceContext* GetContext() const noexcept { return context.Get(); }

private:
    unsigned int width;
    unsigned int height;
    
    HWND windowHandle = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Device>            device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     context;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         swapChain; 
    
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  depthStencilView; 

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
};
