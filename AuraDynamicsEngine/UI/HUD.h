#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "../Mouse.h"
#include "HudImageConfig.h"
#include "Rendering/HudImagePipeline.h"
#include "Rendering/HudImageRenderer.h"
#include "Textures/TextureCache.h"

// =========================================================================================
// HUD PANEL CONSTRUCTION CONTEXT
// =========================================================================================
class HudPanelContext final
{
public:
    HudPanelContext() = default;

    HudPanelContext(const HudPanelContext&) = delete;
    HudPanelContext& operator=(const HudPanelContext&) = delete;
    HudPanelContext(HudPanelContext&&) = delete;
    HudPanelContext& operator=(HudPanelContext&&) = delete;

    template<typename DependencyType>
    void Add(DependencyType& dependency)
    {
        static_assert(
            !std::is_const_v<DependencyType>,
            "HudPanelContext cannot register const dependencies."
        );

        using StoredType = std::remove_cv_t<DependencyType>;

        const std::type_index dependencyType =
            std::type_index(typeid(StoredType));

        const bool inserted =
            dependencies.emplace(
                dependencyType,
                static_cast<void*>(
                    std::addressof(dependency)
                )
            ).second;

        if (!inserted)
        {
            throw std::logic_error(
                "HudPanelContext attempted to register the same dependency type more than once."
            );
        }
    }

    template<typename DependencyType>
    bool Contains() const noexcept
    {
        using StoredType = std::remove_cv_t<DependencyType>;

        return dependencies.find(
            std::type_index(typeid(StoredType))
        ) != dependencies.end();
    }

    template<typename DependencyType>
    DependencyType* Find() const noexcept
    {
        using StoredType = std::remove_cv_t<DependencyType>;

        const auto dependencyIterator =
            dependencies.find(
                std::type_index(typeid(StoredType))
            );

        if (dependencyIterator == dependencies.end())
        {
            return nullptr;
        }

        return static_cast<DependencyType*>(
            dependencyIterator->second
        );
    }

    template<typename DependencyType>
    DependencyType& Require() const
    {
        DependencyType* dependency = Find<DependencyType>();

        if (dependency == nullptr)
        {
            throw std::logic_error(
                "A registered HUD panel requested a dependency that was not added to HudPanelContext."
            );
        }

        return *dependency;
    }

private:
    // Registered dependencies are non-owning and must outlive the panels that use them.
    std::unordered_map<std::type_index, void*> dependencies;
};

// =========================================================================================
// HUD
// =========================================================================================
class HUD final
{
public:
    enum class HudState
    {
        Visible,
        NotVisible
    };

    HUD(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        HudPanelContext& panelContext
    );

    ~HUD();

    HUD(const HUD&) = delete;
    HUD& operator=(const HUD&) = delete;
    HUD(HUD&&) = delete;
    HUD& operator=(HUD&&) = delete;

    template<typename PanelType>
    static bool RegisterHudPanelType()
    {
        static_assert(
            std::is_constructible_v<PanelType, HudPanelContext&>,
            "Registered HUD panels must be constructible from HudPanelContext&."
        );

        static_assert(
            std::is_same_v<
                decltype(
                    std::declval<const PanelType&>().GetImageConfigs()
                ),
                const std::vector<HudImageConfig>&
            >,
            "Registered HUD panels must return const std::vector<HudImageConfig>& from GetImageConfigs()."
        );

        const void* typeKey = GetPanelTypeKey<PanelType>();
        std::vector<PanelFactoryEntry>& registry = GetPanelFactoryRegistry();

        for (const PanelFactoryEntry& entry : registry)
        {
            if (entry.typeKey == typeKey)
            {
                return true;
            }
        }

        if (registry.size() >= MaximumRegisteredPanelTypes)
        {
            throw std::length_error(
                "HUD exceeded the maximum registered panel type count."
            );
        }

        PanelFactoryEntry entry;
        entry.typeKey = typeKey;
        entry.create = &CreatePanelAdapter<PanelType>;

        registry.push_back(entry);

        return true;
    }

    void Update(float deltaTime);

    bool HandleMouseEvent(const Mouse::Event& event) noexcept;
    bool WantsMouseCapture(const Mouse& mouse) const noexcept;

    bool ShowSpeedPanel() noexcept;
    bool ShowLiveMetricsPanel() noexcept;
    bool ShowSystemStatusPanel() noexcept;
    bool ShowAuraEditorPanel() noexcept;
    bool ShowLauncherPanel() noexcept;

    bool IsEditorLauncherOpen() const noexcept;

    void Draw(unsigned int screenWidth, unsigned int screenHeight);

    void SetHudState(HudState state) noexcept;

    HudState GetHudState() const noexcept
    {
        return hudState;
    }

private:
    struct HudPanelElement;

    struct PanelFactoryEntry
    {
        const void* typeKey = nullptr;

        HudPanelElement (*create)(HudPanelContext& context) = nullptr;
    };

    struct HudPanelElement
    {
        void* panel = nullptr;
        const void* typeKey = nullptr;

        void (*destroy)(void* panel) noexcept = nullptr;

        void (*update)(void* panel, float deltaTime) noexcept = nullptr;

        void (*updateLayout)(
            void* panel,
            unsigned int screenWidth,
            unsigned int screenHeight
        ) = nullptr;

        void (*draw)(
            void* panel,
            DirectX::SpriteBatch* spriteBatch,
            DirectX::SpriteFont* spriteFont,
            ID3D11ShaderResourceView* whiteTexture,
            unsigned int screenWidth,
            unsigned int screenHeight,
            TextureCache& textureCache
        ) = nullptr;

        bool (*handleMouseEvent)(
            void* panel,
            const Mouse::Event& event
        ) noexcept = nullptr;

        bool (*wantsMouseCapture)(
            const void* panel,
            const Mouse& mouse
        ) noexcept = nullptr;

        const std::vector<HudImageConfig>& (*getImageConfigs)(
            const void* panel
        ) noexcept = nullptr;

        PanelRectangle (*getImageRectangle)(
            const void* panel,
            const HudImageConfig& config
        ) noexcept = nullptr;
    };

    struct HudImageElement
    {
        const void* ownerPanel = nullptr;

        HudImageConfig config;
        std::unique_ptr<HudImageRenderer> renderer;

        PanelRectangle (*getRectangle)(
            const void* panel,
            const HudImageConfig& config
        ) noexcept = nullptr;
    };

    static std::vector<PanelFactoryEntry>& GetPanelFactoryRegistry();

    void CreateRegisteredPanels(
        ID3D11Device* device,
        HudPanelContext& panelContext
    );

    void RegisterCreatedPanel(
        ID3D11Device* device,
        HudPanelElement panelElement
    );

    void DestroyRegisteredPanels() noexcept;

    template<typename PanelType>
    PanelType* FindPanel() noexcept
    {
        const void* requestedType = GetPanelTypeKey<PanelType>();

        for (HudPanelElement& panelElement : panels)
        {
            if (panelElement.typeKey == requestedType &&
                panelElement.panel != nullptr)
            {
                return static_cast<PanelType*>(
                    panelElement.panel
                );
            }
        }

        return nullptr;
    }

    template<typename PanelType>
    const PanelType* FindPanel() const noexcept
    {
        const void* requestedType = GetPanelTypeKey<PanelType>();

        for (const HudPanelElement& panelElement : panels)
        {
            if (panelElement.typeKey == requestedType &&
                panelElement.panel != nullptr)
            {
                return static_cast<const PanelType*>(
                    panelElement.panel
                );
            }
        }

        return nullptr;
    }

    template<typename PanelType>
    static const void* GetPanelTypeKey() noexcept
    {
        static const std::uint8_t TypeKey = 0u;

        return &TypeKey;
    }

    template<typename PanelType>
    static HudPanelElement CreatePanelAdapter(HudPanelContext& context)
    {
        std::unique_ptr<PanelType> panel =
            std::make_unique<PanelType>(context);

        HudPanelElement result;

        result.panel = panel.get();
        result.typeKey = GetPanelTypeKey<PanelType>();

        result.destroy = &DestroyPanelAdapter<PanelType>;
        result.update = &UpdatePanelAdapter<PanelType>;
        result.updateLayout = &UpdatePanelLayoutAdapter<PanelType>;
        result.draw = &DrawPanelAdapter<PanelType>;

        result.handleMouseEvent =
            &HandlePanelMouseEventAdapter<PanelType>;

        result.wantsMouseCapture =
            &PanelWantsMouseCaptureAdapter<PanelType>;

        result.getImageConfigs =
            &GetPanelImageConfigsAdapter<PanelType>;

        result.getImageRectangle =
            &GetPanelImageRectangleAdapter<PanelType>;

        panel.release();

        return result;
    }

    template<typename PanelType>
    static void DestroyPanelAdapter(void* panel) noexcept
    {
        delete static_cast<PanelType*>(panel);
    }

    template<typename PanelType>
    static void UpdatePanelAdapter(void* panel, float deltaTime) noexcept
    {
        PanelType* typedPanel = static_cast<PanelType*>(panel);

        if (typedPanel != nullptr)
        {
            typedPanel->Update(deltaTime);
        }
    }

    template<typename PanelType>
    static void UpdatePanelLayoutAdapter(
        void* panel,
        unsigned int screenWidth,
        unsigned int screenHeight
    )
    {
        PanelType* typedPanel = static_cast<PanelType*>(panel);

        if (typedPanel != nullptr)
        {
            typedPanel->UpdateLayout(screenWidth, screenHeight);
        }
    }

    template<typename PanelType>
    static void DrawPanelAdapter(
        void* panel,
        DirectX::SpriteBatch* spriteBatch,
        DirectX::SpriteFont* spriteFont,
        ID3D11ShaderResourceView* whiteTexture,
        unsigned int screenWidth,
        unsigned int screenHeight,
        TextureCache& textureCache
    )
    {
        PanelType* typedPanel = static_cast<PanelType*>(panel);

        if (typedPanel != nullptr)
        {
            typedPanel->Draw(
                spriteBatch,
                spriteFont,
                whiteTexture,
                screenWidth,
                screenHeight,
                textureCache
            );
        }
    }

    template<typename PanelType>
    static bool HandlePanelMouseEventAdapter(
        void* panel,
        const Mouse::Event& event
    ) noexcept
    {
        PanelType* typedPanel = static_cast<PanelType*>(panel);

        return typedPanel != nullptr &&
            typedPanel->HandleMouseEvent(event);
    }

    template<typename PanelType>
    static bool PanelWantsMouseCaptureAdapter(
        const void* panel,
        const Mouse& mouse
    ) noexcept
    {
        const PanelType* typedPanel =
            static_cast<const PanelType*>(panel);

        return typedPanel != nullptr &&
            typedPanel->WantsMouseCapture(mouse);
    }

    template<typename PanelType>
    static const std::vector<HudImageConfig>& GetPanelImageConfigsAdapter(
        const void* panel
    ) noexcept
    {
        static const std::vector<HudImageConfig> EmptyConfigs;

        const PanelType* typedPanel =
            static_cast<const PanelType*>(panel);

        if (typedPanel == nullptr)
        {
            return EmptyConfigs;
        }

        return typedPanel->GetImageConfigs();
    }

    template<typename PanelType>
    static PanelRectangle GetPanelImageRectangleAdapter(
        const void* panel,
        const HudImageConfig& config
    ) noexcept
    {
        const PanelType* typedPanel =
            static_cast<const PanelType*>(panel);

        if (typedPanel == nullptr)
        {
            return {};
        }

        return typedPanel->GetImageRectState(config);
    }

    template<typename PanelType>
    bool ShowPanel() noexcept
    {
        PanelType* panel = FindPanel<PanelType>();

        if (panel == nullptr)
        {
            return false;
        }

        return panel->Show();
    }

    void RegisterPanelImages(
        ID3D11Device* device,
        const HudPanelElement& panelElement
    );

    HudImageElement CreatePanelImageElement(
        ID3D11Device* device,
        const HudPanelElement& panelElement,
        const HudImageConfig& config
    );

    static void ValidateImageConfig(const HudImageConfig& config);

    void DrawPanelImage(
        const HudImageElement& imageElement,
        unsigned int screenWidth,
        unsigned int screenHeight
    ) const;

private:
    static constexpr std::size_t MaximumRegisteredPanelTypes = 64u;
    static constexpr std::size_t MaximumRegisteredPanels = 64u;
    static constexpr std::size_t MaximumImagesPerPanel = 64u;

    static constexpr std::size_t MaximumRegisteredImages =
        MaximumRegisteredPanels * MaximumImagesPerPanel;

    static constexpr std::size_t MaximumImagePathLength = 1024u;

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    TextureCache textureCache;
    HudImagePipeline imagePipeline;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> whiteTexture;

    // Graphics owns the device context and must outlive HUD.
    ID3D11DeviceContext* context = nullptr;

    std::vector<HudPanelElement> panels;
    std::vector<HudImageElement> panelImages;

    HudState hudState = HudState::Visible;
};

// =========================================================================================
// AUTOMATIC HUD PANEL REGISTRATION
// =========================================================================================
#define HUD_JOIN_INTERNAL(a, b) a##b
#define HUD_JOIN(a, b) HUD_JOIN_INTERNAL(a, b)

#define REGISTER_HUD_PANEL(PanelType) \
    namespace \
    { \
        [[maybe_unused]] const bool HUD_JOIN(hudPanelRegistered_, __COUNTER__) = \
            HUD::RegisterHudPanelType<PanelType>(); \
    }