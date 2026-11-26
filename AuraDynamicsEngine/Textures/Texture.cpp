#include "Texture.h"
#include <WICTextureLoader.h>
#include <stdexcept>

Texture::Texture(
    ID3D11Device* device, ID3D11DeviceContext* context,
    const std::wstring& filePath)
{
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        device,
        context,
        filePath.c_str(),
        nullptr,
        pTextureView.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error(
            "HRESULT = " +
            std::to_string(hr)
        );
    }
    
    if (FAILED(hr))
    {
        throw std::runtime_error(
    "HRESULT = " +
    std::to_string((long)hr)
);
        
    }
    
    // Creates the sampler desc
    D3D11_SAMPLER_DESC sd = {};

    sd.Filter =
        D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    sd.AddressU =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    sd.AddressV =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    sd.AddressW =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    sd.MaxLOD =
        D3D11_FLOAT32_MAX;

    hr =
        device->CreateSamplerState(
            &sd,
            pSampler.ReleaseAndGetAddressOf()
        );

    if (FAILED(hr))
    {
        throw std::runtime_error(
            "Failed to create sampler."
        );
    }
}

void Texture::Bind( ID3D11DeviceContext* context, UINT slot)
{
    context->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    
    context->PSSetSamplers(slot, 1u, pSampler.GetAddressOf());
}