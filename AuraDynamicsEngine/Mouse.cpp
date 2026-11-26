#include "Mouse.h"
#include <windows.h>

std::optional<Mouse::Event> Mouse::Read() noexcept
{
    if (buffer.empty())
    {
        return std::nullopt;
    }

    Event event = buffer.front();
    buffer.pop();

    return event;
}

void Mouse::Clear() noexcept
{
    buffer = {};
}

void Mouse::ClearDeltas() noexcept
{
    deltaX = 0;
    deltaY = 0;
}

// =========================================================================================
// MOUSE MOVEMENT
// =========================================================================================

void Mouse::OnMouseMove(int newX, int newY) noexcept
{
    x = newX;
    y = newY;

    deltaX += newX - oldX;
    deltaY += newY - oldY;

    oldX = newX;
    oldY = newY;

    buffer.emplace(Event::Type::Move, *this);
    TrimBuffer();
}

void Mouse::SynchronizePosition(int newX, int newY) noexcept
{
    x = newX;
    y = newY;

    oldX = newX;
    oldY = newY;

    ClearDeltas();
}

// =========================================================================================
// MOUSE BUTTON INPUT
// =========================================================================================

void Mouse::OnLeftPressed(int newX, int newY) noexcept
{
    x = newX;
    y = newY;
    leftIsPressed = true;

    buffer.emplace(Event::Type::LPress, *this);
    TrimBuffer();
}

void Mouse::OnLeftReleased(int newX, int newY) noexcept
{
    x = newX;
    y = newY;
    leftIsPressed = false;

    buffer.emplace(Event::Type::LRelease, *this);
    TrimBuffer();
}

void Mouse::OnRightPressed(int newX, int newY) noexcept
{
    x = newX;
    y = newY;
    rightIsPressed = true;

    buffer.emplace(Event::Type::RPress, *this);
    TrimBuffer();
}

void Mouse::OnRightReleased(int newX, int newY) noexcept
{
    x = newX;
    y = newY;
    rightIsPressed = false;

    buffer.emplace(Event::Type::RRelease, *this);
    TrimBuffer();
}

// =========================================================================================
// MOUSE WHEEL INPUT
// =========================================================================================

void Mouse::OnWheelUp(int newX, int newY) noexcept
{
    x = newX;
    y = newY;

    buffer.emplace(Event::Type::WheelUp, *this);
    TrimBuffer();
}

void Mouse::OnWheelDown(int newX, int newY) noexcept
{
    x = newX;
    y = newY;

    buffer.emplace(Event::Type::WheelDown, *this);
    TrimBuffer();
}

void Mouse::OnWheelDelta(int newX, int newY, int delta) noexcept
{
    wheelDeltaCarry += delta;

    while (wheelDeltaCarry >= WHEEL_DELTA)
    {
        wheelDeltaCarry -= WHEEL_DELTA;
        OnWheelUp(newX, newY);
    }

    while (wheelDeltaCarry <= -WHEEL_DELTA)
    {
        wheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown(newX, newY);
    }
}

// =========================================================================================
// WINDOW TRACKING
// =========================================================================================

void Mouse::OnMouseEnter() noexcept
{
    isInWindow = true;

    buffer.emplace(Event::Type::Enter, *this);
    TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
    isInWindow = false;

    buffer.emplace(Event::Type::Leave, *this);
    TrimBuffer();
}

// =========================================================================================
// EVENT BUFFER MANAGEMENT
// =========================================================================================

void Mouse::TrimBuffer() noexcept
{
    while (buffer.size() > BufferCapacity)
    {
        buffer.pop();
    }
}