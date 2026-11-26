#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Texture
{
public:

    Texture(
        ID3D11Device* device, ID3D11DeviceContext* context,
        const std::wstring& filePath
    );

    void Bind(
        ID3D11DeviceContext* context,
        UINT slot = 0u
    );

private:

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
        pTextureView;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>
        pSampler;
};