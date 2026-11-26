#include "Camera.h"

#include "../Keyboard.h"
#include "../Mouse.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <optional>
#include <sstream>
#include <utility>

// =========================================================================================
// CAMERA VALUE SANITIZATION
// =========================================================================================
bool Camera::IsFiniteVector(const DirectX::XMFLOAT3& value) noexcept
{
    return
        std::isfinite(value.x) &&
        std::isfinite(value.y) &&
        std::isfinite(value.z);
}

DirectX::XMFLOAT3 Camera::SanitizeVector(
    const DirectX::XMFLOAT3& value,
    const DirectX::XMFLOAT3& fallback
) noexcept
{
    return IsFiniteVector(value)
        ? value
        : fallback;
}

float Camera::SanitizeFiniteValue(float value, float fallback) noexcept
{
    return std::isfinite(value)
        ? value
        : fallback;
}

DirectX::XMVECTOR Camera::NormalizeVectorOrFallback(
    DirectX::FXMVECTOR value,
    DirectX::FXMVECTOR fallback
) noexcept
{
    const float lengthSquared =
        DirectX::XMVectorGetX(
            DirectX::XMVector3LengthSq(value)
        );

    if (!std::isfinite(lengthSquared) ||
        lengthSquared <= MinimumVectorLengthSquared)
    {
        return DirectX::XMVector3Normalize(fallback);
    }

    return DirectX::XMVector3Normalize(value);
}

// =========================================================================================
// INITIAL CAMERA POSE
// =========================================================================================
void Camera::SetInitialPose(
    const DirectX::XMFLOAT3& initialPosition,
    float initialYaw,
    float initialPitch
) noexcept
{
    position =
        SanitizeVector(
            initialPosition,
            DirectX::XMFLOAT3
            {
                0.0f,
                0.0f,
                0.0f
            }
        );

    yaw = SanitizeFiniteValue(initialYaw, 0.0f);
    pitch = SanitizeFiniteValue(initialPitch, 0.0f);

    yaw = std::remainder(yaw, DirectX::XM_2PI);
    pitch = std::clamp(pitch, -MaximumPitch, MaximumPitch);

    const DirectX::XMVECTOR initialForward =
        CalculateFreeFlightForward();

    DirectX::XMStoreFloat3(
        &forward,
        initialForward
    );
}

// =========================================================================================
// CAMERA MODE CONTROL
// =========================================================================================
void Camera::SetFreeFlight() noexcept
{
    if (mode == Mode::FreeFlight)
    {
        return;
    }

    SyncForwardAngles();

    mode = Mode::FreeFlight;
}

void Camera::FocusOn(const DirectX::XMFLOAT3& targetPosition) noexcept
{
    if (!IsFiniteVector(targetPosition))
    {
        return;
    }

    focusTarget = targetPosition;
    previousFocusTarget = targetPosition;

    const DirectX::XMVECTOR target =
        DirectX::XMLoadFloat3(
            &focusTarget
        );

    const DirectX::XMVECTOR cameraPosition =
        DirectX::XMLoadFloat3(
            &position
        );

    const DirectX::XMVECTOR defaultFocusDirection =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            -1.0f,
            0.0f
        );

    const DirectX::XMVECTOR targetToCamera =
        NormalizeVectorOrFallback(
            DirectX::XMVectorSubtract(
                cameraPosition,
                target
            ),
            defaultFocusDirection
        );

    DirectX::XMStoreFloat3(
        &focusDirection,
        targetToCamera
    );

    mode = Mode::ReturningFocus;
}

void Camera::SetFocusTarget(const DirectX::XMFLOAT3& targetPosition) noexcept
{
    if (!IsFiniteVector(targetPosition))
    {
        return;
    }

    focusTarget = targetPosition;
}

// =========================================================================================
// CENTRAL CAMERA UPDATE
// =========================================================================================
void Camera::Update(
    float deltaTime,
    Keyboard& keyboard,
    Mouse& mouse,
    bool allowMouseInput
) noexcept
{
    if (!std::isfinite(deltaTime) ||
        deltaTime <= 0.0f)
    {
        return;
    }

    deltaTime =
        std::clamp(
            deltaTime,
            0.0f,
            MaximumDeltaTime
        );

    switch (mode)
    {
    case Mode::FreeFlight:
        UpdateFreeFlight(
            deltaTime,
            keyboard,
            mouse,
            allowMouseInput
        );
        break;

    case Mode::ReturningFocus:
        UpdateReturningFocus(deltaTime);
        break;

    case Mode::TrackingFocus:
        UpdateTrackingFocus();
        break;
    }
}

// =========================================================================================
// FREE-FLIGHT CAMERA
// =========================================================================================
void Camera::UpdateFreeFlight(
    float deltaTime,
    Keyboard& keyboard,
    Mouse& mouse,
    bool allowMouseInput
) noexcept
{
    if (enteringSpeed)
    {
        try
        {
            ProcessSpeedEntry(keyboard);
        }
        catch (...)
        {
            enteringSpeed = false;
            speedInput.clear();

            SetCurrentSpeedMessage(
                "Speed entry failed. Current speed remains "
            );
        }

        return;
    }

    if (allowMouseInput &&
        mouse.LeftIsPressed() &&
        mouse.IsInWindow())
    {
        yaw -=
            static_cast<float>(
                mouse.GetDeltaX()
            ) *
            mouseSensitivity;

        pitch -=
            static_cast<float>(
                mouse.GetDeltaY()
            ) *
            mouseSensitivity;
    }

    if (!std::isfinite(yaw))
    {
        yaw = 0.0f;
    }
    else
    {
        yaw =
            std::remainder(
                yaw,
                DirectX::XM_2PI
            );
    }

    pitch =
        std::clamp(
            SanitizeFiniteValue(
                pitch,
                0.0f
            ),
            -MaximumPitch,
            MaximumPitch
        );

    const DirectX::XMVECTOR cameraForward =
        CalculateFreeFlightForward();

    const DirectX::XMVECTOR worldUp =
        DirectX::XMVectorSet(
            0.0f,
            1.0f,
            0.0f,
            0.0f
        );

    const DirectX::XMVECTOR defaultRight =
        DirectX::XMVectorSet(
            1.0f,
            0.0f,
            0.0f,
            0.0f
        );

    const DirectX::XMVECTOR cameraRight =
        NormalizeVectorOrFallback(
            DirectX::XMVector3Cross(
                worldUp,
                cameraForward
            ),
            defaultRight
        );

    DirectX::XMVECTOR movementDirection =
        DirectX::XMVectorZero();

    if (keyboard.KeyIsPressed('W'))
    {
        movementDirection =
            DirectX::XMVectorAdd(
                movementDirection,
                cameraForward
            );
    }

    if (keyboard.KeyIsPressed('S'))
    {
        movementDirection =
            DirectX::XMVectorSubtract(
                movementDirection,
                cameraForward
            );
    }

    if (keyboard.KeyIsPressed('A'))
    {
        movementDirection =
            DirectX::XMVectorSubtract(
                movementDirection,
                cameraRight
            );
    }

    if (keyboard.KeyIsPressed('D'))
    {
        movementDirection =
            DirectX::XMVectorAdd(
                movementDirection,
                cameraRight
            );
    }

    if (keyboard.KeyIsPressed('Q'))
    {
        movementDirection =
            DirectX::XMVectorAdd(
                movementDirection,
                worldUp
            );
    }

    if (keyboard.KeyIsPressed('E'))
    {
        movementDirection =
            DirectX::XMVectorSubtract(
                movementDirection,
                worldUp
            );
    }

    const float movementLengthSquared =
        DirectX::XMVectorGetX(
            DirectX::XMVector3LengthSq(
                movementDirection
            )
        );

    DirectX::XMVECTOR cameraPosition =
        DirectX::XMLoadFloat3(
            &position
        );

    if (std::isfinite(movementLengthSquared) &&
        movementLengthSquared > MinimumVectorLengthSquared)
    {
        movementDirection =
            DirectX::XMVector3Normalize(
                movementDirection
            );

        const float movementDistance =
            movementSpeed *
            deltaTime;

        cameraPosition =
            DirectX::XMVectorAdd(
                cameraPosition,
                DirectX::XMVectorScale(
                    movementDirection,
                    movementDistance
                )
            );
    }

    DirectX::XMStoreFloat3(
        &position,
        cameraPosition
    );

    DirectX::XMStoreFloat3(
        &forward,
        cameraForward
    );
}

// =========================================================================================
// RETURN CAMERA TO MOVING FOCUS TARGET
// =========================================================================================
void Camera::UpdateReturningFocus(float deltaTime) noexcept
{
    const DirectX::XMVECTOR targetPosition =
        DirectX::XMLoadFloat3(
            &focusTarget
        );

    const DirectX::XMVECTOR previousTargetPosition =
        DirectX::XMLoadFloat3(
            &previousFocusTarget
        );

    DirectX::XMVECTOR currentCameraPosition =
        DirectX::XMLoadFloat3(
            &position
        );

    const DirectX::XMVECTOR targetMovement =
        DirectX::XMVectorSubtract(
            targetPosition,
            previousTargetPosition
        );

    currentCameraPosition =
        DirectX::XMVectorAdd(
            currentCameraPosition,
            targetMovement
        );

    const DirectX::XMVECTOR defaultFocusDirection =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            -1.0f,
            0.0f
        );

    const DirectX::XMVECTOR targetToCameraDirection =
        NormalizeVectorOrFallback(
            DirectX::XMLoadFloat3(
                &focusDirection
            ),
            defaultFocusDirection
        );

    DirectX::XMStoreFloat3(
        &focusDirection,
        targetToCameraDirection
    );

    const DirectX::XMVECTOR desiredCameraPosition =
        DirectX::XMVectorAdd(
            targetPosition,
            DirectX::XMVectorScale(
                targetToCameraDirection,
                focusDistance
            )
        );

    const float interpolationAmount =
        1.0f -
        std::exp(
            -returnSharpness *
            deltaTime
        );

    currentCameraPosition =
        DirectX::XMVectorLerp(
            currentCameraPosition,
            desiredCameraPosition,
            interpolationAmount
        );

    DirectX::XMStoreFloat3(
        &position,
        currentCameraPosition
    );

    const DirectX::XMVECTOR defaultForward =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        );

    const DirectX::XMVECTOR directionToTarget =
        NormalizeVectorOrFallback(
            DirectX::XMVectorSubtract(
                targetPosition,
                currentCameraPosition
            ),
            defaultForward
        );

    DirectX::XMStoreFloat3(
        &forward,
        directionToTarget
    );

    previousFocusTarget = focusTarget;

    const DirectX::XMVECTOR remaining =
        DirectX::XMVectorSubtract(
            desiredCameraPosition,
            currentCameraPosition
        );

    const float remainingDistanceSquared =
        DirectX::XMVectorGetX(
            DirectX::XMVector3LengthSq(
                remaining
            )
        );

    const float arrivalDistanceSquared =
        arrivalDistance *
        arrivalDistance;

    if (std::isfinite(remainingDistanceSquared) &&
        remainingDistanceSquared <= arrivalDistanceSquared)
    {
        DirectX::XMStoreFloat3(
            &position,
            desiredCameraPosition
        );

        mode = Mode::TrackingFocus;

        UpdateTrackingFocus();
    }
}

// =========================================================================================
// EXACT TARGET TRACKING
// =========================================================================================
void Camera::UpdateTrackingFocus() noexcept
{
    const DirectX::XMVECTOR targetPosition =
        DirectX::XMLoadFloat3(
            &focusTarget
        );

    const DirectX::XMVECTOR defaultFocusDirection =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            -1.0f,
            0.0f
        );

    const DirectX::XMVECTOR targetToCameraDirection =
        NormalizeVectorOrFallback(
            DirectX::XMLoadFloat3(
                &focusDirection
            ),
            defaultFocusDirection
        );

    DirectX::XMStoreFloat3(
        &focusDirection,
        targetToCameraDirection
    );

    const DirectX::XMVECTOR trackingPosition =
        DirectX::XMVectorAdd(
            targetPosition,
            DirectX::XMVectorScale(
                targetToCameraDirection,
                focusDistance
            )
        );

    DirectX::XMStoreFloat3(
        &position,
        trackingPosition
    );

    const DirectX::XMVECTOR defaultForward =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        );

    const DirectX::XMVECTOR directionToTarget =
        NormalizeVectorOrFallback(
            DirectX::XMVectorSubtract(
                targetPosition,
                trackingPosition
            ),
            defaultForward
        );

    DirectX::XMStoreFloat3(
        &forward,
        directionToTarget
    );

    previousFocusTarget = focusTarget;
}

// =========================================================================================
// FORWARD VECTOR TO FREE-FLIGHT ANGLES
// =========================================================================================
void Camera::SyncForwardAngles() noexcept
{
    const DirectX::XMVECTOR defaultForward =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        );

    const DirectX::XMVECTOR forwardVector =
        NormalizeVectorOrFallback(
            DirectX::XMLoadFloat3(
                &forward
            ),
            defaultForward
        );

    DirectX::XMFLOAT3 normalizedForward = {};

    DirectX::XMStoreFloat3(
        &normalizedForward,
        forwardVector
    );

    normalizedForward.y =
        std::clamp(
            normalizedForward.y,
            -1.0f,
            1.0f
        );

    pitch =
        std::asin(
            normalizedForward.y
        );

    yaw =
        std::atan2(
            normalizedForward.z,
            normalizedForward.x
        );

    pitch =
        std::clamp(
            pitch,
            -MaximumPitch,
            MaximumPitch
        );
}

// =========================================================================================
// FREE-FLIGHT FORWARD VECTOR
// =========================================================================================
DirectX::XMVECTOR Camera::CalculateFreeFlightForward() const noexcept
{
    const DirectX::XMVECTOR cameraForward =
        DirectX::XMVectorSet(
            std::cos(pitch) *
                std::cos(yaw),

            std::sin(pitch),

            std::sin(yaw) *
                std::cos(pitch),

            0.0f
        );

    const DirectX::XMVECTOR defaultForward =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        );

    return NormalizeVectorOrFallback(
        cameraForward,
        defaultForward
    );
}

// =========================================================================================
// CAMERA VIEW MATRIX
// =========================================================================================
DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    const DirectX::XMVECTOR cameraPosition =
        DirectX::XMLoadFloat3(
            &position
        );

    const DirectX::XMVECTOR defaultForward =
        DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        );

    const DirectX::XMVECTOR cameraForward =
        NormalizeVectorOrFallback(
            DirectX::XMLoadFloat3(
                &forward
            ),
            defaultForward
        );

    const DirectX::XMVECTOR worldUp =
        DirectX::XMVectorSet(
            0.0f,
            1.0f,
            0.0f,
            0.0f
        );

    const float verticalAlignment =
        std::abs(
            DirectX::XMVectorGetX(
                DirectX::XMVector3Dot(
                    cameraForward,
                    worldUp
                )
            )
        );

    const DirectX::XMVECTOR cameraUp =
        verticalAlignment >= ParallelDirectionThreshold
        ? DirectX::XMVectorSet(
            0.0f,
            0.0f,
            1.0f,
            0.0f
        )
        : worldUp;

    return DirectX::XMMatrixLookToLH(
        cameraPosition,
        cameraForward,
        cameraUp
    );
}

// =========================================================================================
// SPEED ENTRY MESSAGE FORMATTING
// =========================================================================================
std::string Camera::FormatSpeed(float speed)
{
    std::ostringstream stream;

    stream <<
        std::fixed <<
        std::setprecision(2) <<
        speed;

    std::string formattedSpeed =
        stream.str();

    while (!formattedSpeed.empty() &&
        formattedSpeed.back() == '0')
    {
        formattedSpeed.pop_back();
    }

    if (!formattedSpeed.empty() &&
        formattedSpeed.back() == '.')
    {
        formattedSpeed.pop_back();
    }

    return formattedSpeed;
}

void Camera::SetSpeedMessage(std::string_view message) noexcept
{
    try
    {
        speedMessage.assign(
            message.data(),
            message.size()
        );
    }
    catch (...)
    {
        speedMessage.clear();
    }
}

void Camera::SetCurrentSpeedMessage(std::string_view prefix) noexcept
{
    try
    {
        speedMessage.assign(
            prefix.data(),
            prefix.size()
        );

        speedMessage +=
            FormatSpeed(
                movementSpeed
            );
    }
    catch (...)
    {
        speedMessage.clear();
    }
}

// =========================================================================================
// SPEED ENTRY STATE
// =========================================================================================
void Camera::BeginSpeedEntry() noexcept
{
    if (enteringSpeed)
    {
        return;
    }

    enteringSpeed = true;
    speedInput.clear();

    try
    {
        speedMessage =
            "Enter movement speed (" +
            FormatSpeed(MinimumMovementSpeed) +
            " - " +
            FormatSpeed(MaximumMovementSpeed) +
            ")";
    }
    catch (...)
    {
        speedMessage.clear();
    }
}

void Camera::CancelSpeedEntry() noexcept
{
    enteringSpeed = false;
    speedInput.clear();

    SetCurrentSpeedMessage(
        "Speed unchanged: "
    );
}

void Camera::SubmitSpeedEntry() noexcept
{
    if (speedInput.empty() ||
        speedInput == ".")
    {
        enteringSpeed = false;
        speedInput.clear();

        SetCurrentSpeedMessage(
            "Invalid speed. Current speed remains "
        );

        return;
    }

    try
    {
        std::size_t consumedCharacters = 0u;

        const float requestedSpeed =
            std::stof(
                speedInput,
                &consumedCharacters
            );

        const bool validSpeed =
            consumedCharacters == speedInput.size() &&
            std::isfinite(requestedSpeed) &&
            requestedSpeed >= MinimumMovementSpeed &&
            requestedSpeed <= MaximumMovementSpeed;

        if (!validSpeed)
        {
            enteringSpeed = false;
            speedInput.clear();

            SetCurrentSpeedMessage(
                "Invalid speed. Current speed remains "
            );

            return;
        }

        movementSpeed = requestedSpeed;

        enteringSpeed = false;
        speedInput.clear();

        SetCurrentSpeedMessage(
            "Movement speed changed to "
        );
    }
    catch (...)
    {
        enteringSpeed = false;
        speedInput.clear();

        SetCurrentSpeedMessage(
            "Invalid speed. Current speed remains "
        );
    }
}

// =========================================================================================
// SPEED ENTRY INPUT
// =========================================================================================
void Camera::ProcessSpeedEntry(Keyboard& keyboard)
{
    while (!keyboard.CharIsEmpty())
    {
        const std::optional<wchar_t> characterOptional =
            keyboard.ReadChar();

        if (!characterOptional.has_value())
        {
            continue;
        }

        const wchar_t character =
            characterOptional.value();

        if (character == L'\r')
        {
            SubmitSpeedEntry();
            return;
        }

        if (character == L'\b')
        {
            if (!speedInput.empty())
            {
                speedInput.pop_back();
            }

            continue;
        }

        if (speedInput.size() >=
            MaximumSpeedInputCharacters)
        {
            continue;
        }

        if (character >= L'0' &&
            character <= L'9')
        {
            speedInput.push_back(
                static_cast<char>(
                    character
                )
            );

            continue;
        }

        if (character == L'.' &&
            speedInput.find('.') == std::string::npos)
        {
            speedInput.push_back('.');
        }
    }
}