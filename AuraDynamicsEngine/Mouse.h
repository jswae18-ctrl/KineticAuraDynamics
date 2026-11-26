#pragma once
#include <cstddef>
#include <optional>
#include <queue>

class Mouse
{
    friend class MainWindow;

public:
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave,
            Invalid
        };

        Event(Type type, const Mouse& mouse) noexcept
            :
            type(type),
            leftIsPressed(mouse.leftIsPressed),
            rightIsPressed(mouse.rightIsPressed),
            x(mouse.x),
            y(mouse.y)
        {
        }

        Type GetType() const noexcept { return type; }

        bool LeftIsPressed() const noexcept { return leftIsPressed; }
        bool RightIsPressed() const noexcept { return rightIsPressed; }

        int GetPosX() const noexcept { return x; }
        int GetPosY() const noexcept { return y; }

    private:
        Type type = Type::Invalid;

        bool leftIsPressed = false;
        bool rightIsPressed = false;

        int x = 0;
        int y = 0;
    };

public:
    Mouse() = default;

    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    Mouse(Mouse&&) = delete;
    Mouse& operator=(Mouse&&) = delete;

    int GetPosX() const noexcept { return x; }
    int GetPosY() const noexcept { return y; }

    int GetDeltaX() const noexcept { return deltaX; }
    int GetDeltaY() const noexcept { return deltaY; }

    bool LeftIsPressed() const noexcept { return leftIsPressed; }
    bool RightIsPressed() const noexcept { return rightIsPressed; }
    bool IsInWindow() const noexcept { return isInWindow; }

    bool IsEmpty() const noexcept { return buffer.empty(); }

    std::optional<Event> Read() noexcept;

    void Clear() noexcept;
    void ClearDeltas() noexcept;

private:
    void OnMouseMove(int newX, int newY) noexcept;

    void OnLeftPressed(int newX, int newY) noexcept;
    void OnLeftReleased(int newX, int newY) noexcept;

    void OnRightPressed(int newX, int newY) noexcept;
    void OnRightReleased(int newX, int newY) noexcept;

    void OnWheelUp(int newX, int newY) noexcept;
    void OnWheelDown(int newX, int newY) noexcept;
    void OnWheelDelta(int newX, int newY, int delta) noexcept;

    void OnMouseEnter() noexcept;
    void OnMouseLeave() noexcept;

    void SynchronizePosition(int newX, int newY) noexcept;
    void TrimBuffer() noexcept;

private:
    static constexpr std::size_t BufferCapacity = 16u;

    int x = 0;
    int y = 0;

    int oldX = 0;
    int oldY = 0;

    int deltaX = 0;
    int deltaY = 0;

    int wheelDeltaCarry = 0;

    bool leftIsPressed = false;
    bool rightIsPressed = false;
    bool isInWindow = false;

    std::queue<Event> buffer;
};