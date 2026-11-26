#include "HUD.h"

#include "Editor/AuraEditor.h"
#include "Editor/EditorLauncher.h"
#include "Panels/CameraSpeedPanel.h"
#include "Panels/CPUMetrics/LiveMetrics.h"
#include "Panels/CPUMetrics/SystemStatus.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
    ID3D11Device* ValidateDevice(ID3D11Device* device)
    {
        if (device == nullptr)
        {
            throw std::invalid_argument(
                "HUD requires a valid Direct3D device."
            );
        }

        return device;
    }

    ID3D11DeviceContext* ValidateDeviceContext(ID3D11DeviceContext* context)
    {
        if (context == nullptr)
        {
            throw std::invalid_argument(
                "HUD requires a valid Direct3D device context."
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
}

// =========================================================================================
// HUD CONSTRUCTION
// =========================================================================================
HUD::HUD(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    HudPanelContext& panelContext
)
    :
    textureCache(ValidateDevice(device)),
    imagePipeline(ValidateDevice(device)),
    context(ValidateDeviceContext(context))
{
    panels.reserve(8u);
    panelImages.reserve(32u);

    spriteBatch =
        std::make_unique<DirectX::SpriteBatch>(
            context
        );

    spriteFont =
        std::make_unique<DirectX::SpriteFont>(
            device,
            L"Resources\\Fonts\\AuraMono.spritefont"
        );

    const std::uint32_t whitePixel = 0xFFFFFFFFu;

    D3D11_TEXTURE2D_DESC textureDescription = {};
    textureDescription.Width = 1u;
    textureDescription.Height = 1u;
    textureDescription.MipLevels = 1u;
    textureDescription.ArraySize = 1u;
    textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDescription.SampleDesc.Count = 1u;
    textureDescription.Usage = D3D11_USAGE_IMMUTABLE;
    textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = &whitePixel;
    initialData.SysMemPitch = sizeof(whitePixel);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> whiteTextureResource;

    ThrowIfFailed(
        device->CreateTexture2D(
            &textureDescription,
            &initialData,
            whiteTextureResource.GetAddressOf()
        ),
        "Failed to create the HUD white texture."
    );

    ThrowIfFailed(
        device->CreateShaderResourceView(
            whiteTextureResource.Get(),
            nullptr,
            whiteTexture.GetAddressOf()
        ),
        "Failed to create the HUD white texture shader-resource view."
    );

    try
    {
        CreateRegisteredPanels(device, panelContext);
    }
    catch (...)
    {
        panelImages.clear();
        DestroyRegisteredPanels();

        throw;
    }
}

// =========================================================================================
// HUD DESTRUCTION
// =========================================================================================
HUD::~HUD()
{
    panelImages.clear();
    DestroyRegisteredPanels();
}

// =========================================================================================
// PANEL FACTORY REGISTRY
// =========================================================================================
std::vector<HUD::PanelFactoryEntry>& HUD::GetPanelFactoryRegistry()
{
    static std::vector<PanelFactoryEntry> registry;

    return registry;
}

// =========================================================================================
// REGISTERED PANEL CONSTRUCTION
// =========================================================================================
void HUD::CreateRegisteredPanels(
    ID3D11Device* device,
    HudPanelContext& panelContext
)
{
    if (device == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot create registered panels without a Direct3D device."
        );
    }

    const std::vector<PanelFactoryEntry>& registry =
        GetPanelFactoryRegistry();

    if (registry.size() > MaximumRegisteredPanels)
    {
        throw std::length_error(
            "HUD panel factory count exceeds the supported panel count."
        );
    }

    for (const PanelFactoryEntry& factoryEntry : registry)
    {
        if (factoryEntry.typeKey == nullptr ||
            factoryEntry.create == nullptr)
        {
            throw std::logic_error(
                "HUD encountered an invalid panel factory entry."
            );
        }

        HudPanelElement panelElement =
            factoryEntry.create(panelContext);

        try
        {
            RegisterCreatedPanel(device, panelElement);

            panelElement.panel = nullptr;
        }
        catch (...)
        {
            if (panelElement.panel != nullptr &&
                panelElement.destroy != nullptr)
            {
                panelElement.destroy(panelElement.panel);
                panelElement.panel = nullptr;
            }

            throw;
        }
    }
}

// =========================================================================================
// PANEL REGISTRATION
// =========================================================================================
void HUD::RegisterCreatedPanel(
    ID3D11Device* device,
    HudPanelElement panelElement
)
{
    if (device == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot register a panel without a Direct3D device."
        );
    }

    if (panelElement.panel == nullptr ||
        panelElement.typeKey == nullptr ||
        panelElement.destroy == nullptr ||
        panelElement.update == nullptr ||
        panelElement.updateLayout == nullptr ||
        panelElement.draw == nullptr ||
        panelElement.handleMouseEvent == nullptr ||
        panelElement.wantsMouseCapture == nullptr ||
        panelElement.getImageConfigs == nullptr ||
        panelElement.getImageRectangle == nullptr)
    {
        throw std::invalid_argument(
            "HUD received an incomplete panel lifecycle entry."
        );
    }

    if (panels.size() >= MaximumRegisteredPanels)
    {
        throw std::length_error(
            "HUD exceeded the maximum registered panel count."
        );
    }

    for (const HudPanelElement& existingPanel : panels)
    {
        if (existingPanel.typeKey == panelElement.typeKey)
        {
            throw std::logic_error(
                "HUD attempted to create the same panel type more than once."
            );
        }
    }

    const std::size_t originalImageCount = panelImages.size();

    try
    {
        RegisterPanelImages(device, panelElement);

        panels.push_back(panelElement);
    }
    catch (...)
    {
        panelImages.resize(originalImageCount);

        throw;
    }
}

// =========================================================================================
// REGISTERED PANEL DESTRUCTION
// =========================================================================================
void HUD::DestroyRegisteredPanels() noexcept
{
    for (auto panelIterator = panels.rbegin();
         panelIterator != panels.rend();
         ++panelIterator)
    {
        HudPanelElement& panelElement = *panelIterator;

        if (panelElement.panel != nullptr &&
            panelElement.destroy != nullptr)
        {
            panelElement.destroy(panelElement.panel);
            panelElement.panel = nullptr;
        }
    }

    panels.clear();
}

// =========================================================================================
// PANEL IMAGE REGISTRATION
// =========================================================================================
void HUD::RegisterPanelImages(
    ID3D11Device* device,
    const HudPanelElement& panelElement
)
{
    if (device == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot register panel images without a Direct3D device."
        );
    }

    if (panelElement.panel == nullptr ||
        panelElement.getImageConfigs == nullptr ||
        panelElement.getImageRectangle == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot register images for an incomplete panel entry."
        );
    }

    const std::vector<HudImageConfig>& imageConfigs =
        panelElement.getImageConfigs(panelElement.panel);

    if (imageConfigs.size() > MaximumImagesPerPanel)
    {
        throw std::length_error(
            "HUD panel exceeded the maximum configured image count."
        );
    }

    if (panelImages.size() > MaximumRegisteredImages ||
        imageConfigs.size() >
            MaximumRegisteredImages - panelImages.size())
    {
        throw std::length_error(
            "HUD exceeded the maximum total registered image count."
        );
    }

    std::vector<HudImageElement> pendingImages;
    pendingImages.reserve(imageConfigs.size());

    for (const HudImageConfig& config : imageConfigs)
    {
        ValidateImageConfig(config);

        pendingImages.push_back(
            CreatePanelImageElement(
                device,
                panelElement,
                config
            )
        );
    }

    panelImages.reserve(
        panelImages.size() +
        pendingImages.size()
    );

    for (HudImageElement& imageElement : pendingImages)
    {
        panelImages.push_back(
            std::move(imageElement)
        );
    }
}

// =========================================================================================
// PANEL IMAGE CREATION
// =========================================================================================
HUD::HudImageElement HUD::CreatePanelImageElement(
    ID3D11Device* device,
    const HudPanelElement& panelElement,
    const HudImageConfig& config
)
{
    if (device == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot create a panel image without a Direct3D device."
        );
    }

    if (panelElement.panel == nullptr ||
        panelElement.getImageRectangle == nullptr)
    {
        throw std::invalid_argument(
            "HUD cannot create an image for an incomplete panel entry."
        );
    }

    ID3D11ShaderResourceView* textureView =
        textureCache.GetTexture(config.assetPath);

    if (textureView == nullptr)
    {
        throw std::runtime_error(
            "HUD could not acquire the configured image texture."
        );
    }

    HudImageElement imageElement;

    imageElement.ownerPanel = panelElement.panel;
    imageElement.config = config;

    imageElement.renderer =
        std::make_unique<HudImageRenderer>(
            device,
            imagePipeline,
            textureView,
            config.tintColor
        );

    imageElement.getRectangle =
        panelElement.getImageRectangle;

    return imageElement;
}

// =========================================================================================
// IMAGE CONFIGURATION VALIDATION
// =========================================================================================
void HUD::ValidateImageConfig(const HudImageConfig& config)
{
    if (config.assetPath.empty())
    {
        throw std::invalid_argument(
            "HUD image configuration has an empty asset path."
        );
    }

    if (config.assetPath.size() > MaximumImagePathLength)
    {
        throw std::length_error(
            "HUD image path exceeds the supported length."
        );
    }

    const auto isFinite =
        [](float value) noexcept
        {
            return std::isfinite(value);
        };

    if (!isFinite(config.tintColor.x) ||
        !isFinite(config.tintColor.y) ||
        !isFinite(config.tintColor.z) ||
        !isFinite(config.tintColor.w))
    {
        throw std::invalid_argument(
            "HUD image tint contains a non-finite component."
        );
    }

    if (config.tintColor.x < 0.0f ||
        config.tintColor.x > 1.0f ||
        config.tintColor.y < 0.0f ||
        config.tintColor.y > 1.0f ||
        config.tintColor.z < 0.0f ||
        config.tintColor.z > 1.0f ||
        config.tintColor.w < 0.0f ||
        config.tintColor.w > 1.0f)
    {
        throw std::out_of_range(
            "HUD image tint components must be between zero and one."
        );
    }

    if (!config.followsWindowRectangle)
    {
        if (!isFinite(config.localRectangle.x) ||
            !isFinite(config.localRectangle.y) ||
            !isFinite(config.localRectangle.width) ||
            !isFinite(config.localRectangle.height))
        {
            throw std::invalid_argument(
                "HUD image rectangle contains a non-finite value."
            );
        }

        if (config.localRectangle.width <= 0.0f ||
            config.localRectangle.height <= 0.0f)
        {
            throw std::out_of_range(
                "HUD image width and height must be greater than zero."
            );
        }
    }
}

// =========================================================================================
// PANEL IMAGE DRAWING
// =========================================================================================
void HUD::DrawPanelImage(
    const HudImageElement& imageElement,
    unsigned int screenWidth,
    unsigned int screenHeight
) const
{
    if (imageElement.ownerPanel == nullptr ||
        !imageElement.renderer ||
        imageElement.getRectangle == nullptr ||
        context == nullptr ||
        screenWidth == 0u ||
        screenHeight == 0u)
    {
        return;
    }

    const PanelRectangle rectangle =
        imageElement.getRectangle(
            imageElement.ownerPanel,
            imageElement.config
        );

    if (!std::isfinite(rectangle.x) ||
        !std::isfinite(rectangle.y) ||
        !std::isfinite(rectangle.width) ||
        !std::isfinite(rectangle.height) ||
        rectangle.width <= 0.0f ||
        rectangle.height <= 0.0f)
    {
        return;
    }

    imageElement.renderer->Draw(
        context,
        rectangle.x,
        rectangle.y,
        rectangle.width,
        rectangle.height,
        screenWidth,
        screenHeight
    );
}

// =========================================================================================
// HUD UPDATE
// =========================================================================================
void HUD::Update(float deltaTime)
{
    if (hudState == HudState::NotVisible)
    {
        return;
    }

    if (!std::isfinite(deltaTime) ||
        deltaTime < 0.0f)
    {
        return;
    }

    for (const HudPanelElement& panelElement : panels)
    {
        if (panelElement.panel == nullptr ||
            panelElement.update == nullptr)
        {
            continue;
        }

        panelElement.update(
            panelElement.panel,
            deltaTime
        );
    }

    AuraEditor* auraEditor = FindPanel<AuraEditor>();
    EditorLauncher* launcher = FindPanel<EditorLauncher>();

    if (auraEditor != nullptr &&
        launcher != nullptr &&
        auraEditor->ConsumeLauncherRequest())
    {
        launcher->Show();
        auraEditor->Hide();
    }

    if (launcher != nullptr &&
        auraEditor != nullptr &&
        launcher->ConsumeCloseRequest())
    {
        auraEditor->Show();
    }
}

// =========================================================================================
// MOUSE EVENT HANDLING
// =========================================================================================
bool HUD::HandleMouseEvent(const Mouse::Event& event) noexcept
{
    if (hudState == HudState::NotVisible)
    {
        return false;
    }

    for (auto panelIterator = panels.rbegin();
         panelIterator != panels.rend();
         ++panelIterator)
    {
        HudPanelElement& panelElement = *panelIterator;

        if (panelElement.panel == nullptr ||
            panelElement.handleMouseEvent == nullptr)
        {
            continue;
        }

        if (panelElement.handleMouseEvent(
            panelElement.panel,
            event
        ))
        {
            return true;
        }
    }

    return false;
}

// =========================================================================================
// PANEL ACCESS
// =========================================================================================
bool HUD::ShowSpeedPanel() noexcept
{
    return ShowPanel<CameraSpeedPanel>();
}

bool HUD::ShowLiveMetricsPanel() noexcept
{
    return ShowPanel<LiveMetrics>();
}

bool HUD::ShowSystemStatusPanel() noexcept
{
    return ShowPanel<SystemStatus>();
}

bool HUD::ShowAuraEditorPanel() noexcept
{
    return ShowPanel<AuraEditor>();
}

bool HUD::ShowLauncherPanel() noexcept
{
    return ShowPanel<EditorLauncher>();
}

bool HUD::IsEditorLauncherOpen() const noexcept
{
    const EditorLauncher* launcher =
        FindPanel<EditorLauncher>();

    return launcher != nullptr &&
        launcher->IsVisible();
}

// =========================================================================================
// MOUSE CAPTURE
// =========================================================================================
bool HUD::WantsMouseCapture(const Mouse& mouse) const noexcept
{
    if (hudState == HudState::NotVisible)
    {
        return false;
    }

    for (auto panelIterator = panels.rbegin();
         panelIterator != panels.rend();
         ++panelIterator)
    {
        const HudPanelElement& panelElement = *panelIterator;

        if (panelElement.panel == nullptr ||
            panelElement.wantsMouseCapture == nullptr)
        {
            continue;
        }

        if (panelElement.wantsMouseCapture(
            panelElement.panel,
            mouse
        ))
        {
            return true;
        }
    }

    return false;
}

// =========================================================================================
// HUD DRAWING
// =========================================================================================
void HUD::Draw(unsigned int screenWidth, unsigned int screenHeight)
{
    if (hudState == HudState::NotVisible)
    {
        return;
    }

    if (!spriteBatch ||
        !spriteFont ||
        !whiteTexture ||
        context == nullptr ||
        screenWidth == 0u ||
        screenHeight == 0u)
    {
        return;
    }

    for (const HudPanelElement& panelElement : panels)
    {
        if (panelElement.panel == nullptr ||
            panelElement.updateLayout == nullptr)
        {
            continue;
        }

        panelElement.updateLayout(
            panelElement.panel,
            screenWidth,
            screenHeight
        );
    }

    for (const HudImageElement& imageElement : panelImages)
    {
        DrawPanelImage(
            imageElement,
            screenWidth,
            screenHeight
        );
    }

    spriteBatch->Begin();

    for (const HudPanelElement& panelElement : panels)
    {
        if (panelElement.panel == nullptr ||
            panelElement.draw == nullptr)
        {
            continue;
        }

        panelElement.draw(
            panelElement.panel,
            spriteBatch.get(),
            spriteFont.get(),
            whiteTexture.Get(),
            screenWidth,
            screenHeight,
            textureCache
        );
    }

    spriteBatch->End();
}

// =========================================================================================
// HUD STATE CONTROL
// =========================================================================================
void HUD::SetHudState(HudState state) noexcept
{
    hudState = state;
}