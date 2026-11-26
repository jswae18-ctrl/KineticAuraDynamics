#pragma once
#include <cstddef>
#include <string>
#include <string_view>
#include <DirectXMath.h>

class Keyboard;
class Mouse;

// =========================================================================================
// CAMERA
// =========================================================================================
class Camera final
{
public:
    enum class Mode
    {
        FreeFlight,
        ReturningFocus,
        TrackingFocus
    };

    Camera() = default;

    ~Camera() = default;

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;

    void SetInitialPose(
        const DirectX::XMFLOAT3& initialPosition,
        float initialYaw,
        float initialPitch
    ) noexcept;

    void SetFreeFlight() noexcept;

    void FocusOn(const DirectX::XMFLOAT3& targetPosition) noexcept;

    void SetFocusTarget(const DirectX::XMFLOAT3& targetPosition) noexcept;

    void Update(
        float deltaTime,
        Keyboard& keyboard,
        Mouse& mouse,
        bool allowMouseInput
    ) noexcept;

    DirectX::XMMATRIX GetViewMatrix() const noexcept;

    const DirectX::XMFLOAT3& GetPosition() const noexcept
    {
        return position;
    }

    Mode GetMode() const noexcept
    {
        return mode;
    }

    bool IsFreeFlight() const noexcept
    {
        return mode == Mode::FreeFlight;
    }

    void BeginSpeedEntry() noexcept;
    void CancelSpeedEntry() noexcept;
    void SubmitSpeedEntry() noexcept;

    void ProcessSpeedEntry(Keyboard& keyboard);

    bool IsEnteringSpeed() const noexcept
    {
        return enteringSpeed;
    }

    float GetMovementSpeed() const noexcept
    {
        return movementSpeed;
    }

    const std::string& GetSpeedInput() const noexcept
    {
        return speedInput;
    }

    const std::string& GetSpeedMessage() const noexcept
    {
        return speedMessage;
    }

private:
    void UpdateFreeFlight(
        float deltaTime,
        Keyboard& keyboard,
        Mouse& mouse,
        bool allowMouseInput
    ) noexcept;

    void UpdateReturningFocus(float deltaTime) noexcept;
    void UpdateTrackingFocus() noexcept;

    void SyncForwardAngles() noexcept;

    DirectX::XMVECTOR CalculateFreeFlightForward() const noexcept;

    static bool IsFiniteVector(
        const DirectX::XMFLOAT3& value
    ) noexcept;

    static DirectX::XMFLOAT3 SanitizeVector(
        const DirectX::XMFLOAT3& value,
        const DirectX::XMFLOAT3& fallback
    ) noexcept;

    static float SanitizeFiniteValue(float value, float fallback) noexcept;

    static DirectX::XMVECTOR NormalizeVectorOrFallback(
        DirectX::FXMVECTOR value,
        DirectX::FXMVECTOR fallback
    ) noexcept;

    static std::string FormatSpeed(float speed);

    void SetSpeedMessage(std::string_view message) noexcept;
    void SetCurrentSpeedMessage(std::string_view prefix) noexcept;

private:
    Mode mode = Mode::FreeFlight;

    DirectX::XMFLOAT3 position =
    {
        0.0f,
        0.0f,
        0.0f
    };

    DirectX::XMFLOAT3 forward =
    {
        0.0f,
        0.0f,
        1.0f
    };

    DirectX::XMFLOAT3 focusTarget =
    {
        0.0f,
        0.0f,
        0.0f
    };

    DirectX::XMFLOAT3 focusDirection =
    {
        0.0f,
        0.0f,
        -1.0f
    };

    DirectX::XMFLOAT3 previousFocusTarget =
    {
        0.0f,
        0.0f,
        0.0f
    };

    float focusDistance = 4.0f;

    float yaw = 0.0f;
    float pitch = 0.0f;

    float movementSpeed = 1.5f;
    float mouseSensitivity = 0.002f;

    float returnSharpness = 2.5f;
    float arrivalDistance = 0.02f;

    bool enteringSpeed = false;

    std::string speedInput;
    std::string speedMessage;

    static constexpr float MinimumMovementSpeed = 0.1f;
    static constexpr float MaximumMovementSpeed = 100.0f;

    static constexpr float MaximumPitch = 1.5533f;
    static constexpr float MaximumDeltaTime = 0.1f;

    static constexpr float MinimumVectorLengthSquared = 0.000001f;
    static constexpr float ParallelDirectionThreshold = 0.999f;

    static constexpr std::size_t MaximumSpeedInputCharacters = 16u;
};