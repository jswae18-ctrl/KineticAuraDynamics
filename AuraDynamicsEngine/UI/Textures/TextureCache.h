#pragma once
#include <string>
#include <unordered_map>
#include <d3d11.h>
#include <wrl/client.h>

// =========================================================================================
// TEXTURE DIMENSIONS
// =========================================================================================
struct TextureDimensions
{
    unsigned int width = 0u;
    unsigned int height = 0u;
};

// =========================================================================================
// UI TEXTURE CACHE
// =========================================================================================
class TextureCache
{
public:
    explicit TextureCache(ID3D11Device* device);

    ~TextureCache() = default;

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache(TextureCache&&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    ID3D11ShaderResourceView* GetTexture(
        const std::wstring& assetPath
    );

    TextureDimensions GetTextureDimensions(
        const std::wstring& assetPath
    );

    void Clear() noexcept;

private:
    struct CachedTexture
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
        TextureDimensions dimensions;
    };

private:
    // The device is borrowed from Graphics and must outlive the cache.
    ID3D11Device* device = nullptr;

    std::unordered_map<std::wstring, CachedTexture> textures;
};