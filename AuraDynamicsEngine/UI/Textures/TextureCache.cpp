#include "TextureCache.h"
#include <stdexcept>
#include <utility>

#include <WICTextureLoader.h>

namespace
{
    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "TextureCache requires a valid Direct3D device."
            );
        }

        return device;
    }

    void ValidateAssetPath(
        const std::wstring& assetPath
    )
    {
        if (assetPath.empty())
        {
            throw std::invalid_argument(
                "TextureCache requires a non-empty asset path."
            );
        }
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
// TEXTURE CACHE CONSTRUCTION
// =========================================================================================
TextureCache::TextureCache(ID3D11Device* device)
    :
    device(ValidateDevice(device))
{
}

// =========================================================================================
// GETS OR LOADS A TEXTURE
// =========================================================================================
ID3D11ShaderResourceView* TextureCache::GetTexture(
    const std::wstring& assetPath
)
{
    ValidateAssetPath(assetPath);

    const auto existingTexture = textures.find(assetPath);

    if (existingTexture != textures.end())
    {
        return existingTexture->second.resourceView.Get();
    }

    // =====================================================================================
    // TEXTURE LOADING
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;

    ThrowIfFailed(
        DirectX::CreateWICTextureFromFile(
            device,
            assetPath.c_str(),
            textureResource.GetAddressOf(),
            resourceView.GetAddressOf()
        ),
        "TextureCache failed to load a UI texture."
    );

    if (!textureResource || !resourceView)
    {
        throw std::runtime_error(
            "TextureCache received incomplete texture resources."
        );
    }

    // =====================================================================================
    // TEXTURE DIMENSIONS
    // =====================================================================================
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;

    ThrowIfFailed(
        textureResource.As(&texture2D),
        "TextureCache loaded a resource that is not a two-dimensional texture."
    );

    D3D11_TEXTURE2D_DESC textureDescription = {};
    texture2D->GetDesc(&textureDescription);

    if (textureDescription.Width == 0u ||
        textureDescription.Height == 0u)
    {
        throw std::runtime_error(
            "TextureCache loaded a texture with invalid dimensions."
        );
    }
    
    // Store the view and its dimensions under one asset-path entry.
    CachedTexture cachedTexture;
    cachedTexture.resourceView = std::move(resourceView);
    cachedTexture.dimensions =
    {
        textureDescription.Width,
        textureDescription.Height
    };

    const auto storedTexture =
        textures.emplace(
            assetPath,
            std::move(cachedTexture)
        );

    if (!storedTexture.first->second.resourceView)
    {
        throw std::logic_error(
            "TextureCache contains an invalid texture entry."
        );
    }

    return storedTexture.first->second.resourceView.Get();
}

// =========================================================================================
// GETS THE ORIGINAL TEXTURE DIMENSIONS
// =========================================================================================
TextureDimensions TextureCache::GetTextureDimensions(
    const std::wstring& assetPath
)
{
    ValidateAssetPath(assetPath);

    // Ensure the complete cache entry exists before reading its dimensions.
    GetTexture(assetPath);

    const auto existingTexture = textures.find(assetPath);

    if (existingTexture == textures.end())
    {
        throw std::logic_error(
            "TextureCache lost a texture after successful loading."
        );
    }

    return existingTexture->second.dimensions;
}

// =========================================================================================
// CLEARS ALL CACHED TEXTURES
// =========================================================================================
void TextureCache::Clear() noexcept
{
    textures.clear();
}