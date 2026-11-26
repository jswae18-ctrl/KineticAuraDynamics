#pragma once
#include <bitset>
#include <cstddef>
#include <optional>
#include <queue>


class Keyboard
{
    friend class MainWindow;

public:
    class Event
    {
    public:
        enum class Type
        {
            Press,
            Release,
            Invalid
        };

        Event() noexcept = default;

        Event(Type type, unsigned char code) noexcept
            :
            type(type),
            code(code)
        {
        }

        Type GetType() const noexcept { return type; }

        bool IsPress() const noexcept { return type == Type::Press; }
        bool IsRelease() const noexcept { return type == Type::Release; }
        bool IsValid() const noexcept { return type != Type::Invalid; }

        unsigned char GetCode() const noexcept { return code; }

    private:
        Type type = Type::Invalid;
        unsigned char code = 0u;
    };

public:
    Keyboard() = default;

    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;
    Keyboard(Keyboard&&) = delete;
    Keyboard& operator=(Keyboard&&) = delete;

    bool KeyIsPressed(unsigned char keyCode) const noexcept;
    bool KeyIsEmpty() const noexcept;

    std::optional<Event> ReadKey() noexcept;

    void FlushKey() noexcept;

    bool CharIsEmpty() const noexcept;

    std::optional<wchar_t> ReadChar() noexcept;

    void FlushChar() noexcept;
    void Flush() noexcept;

    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;

private:
    void OnKeyPressed(unsigned char keyCode) noexcept;
    void OnKeyReleased(unsigned char keyCode) noexcept;
    void OnChar(wchar_t character) noexcept;

    void ClearState() noexcept;

    template<typename T>
    static void TrimBuffer(std::queue<T>& buffer) noexcept
    {
        while (buffer.size() > BufferCapacity)
        {
            buffer.pop();
        }
    }

private:
    static constexpr std::size_t KeyCount = 256u;
    static constexpr std::size_t BufferCapacity = 16u;

    bool autorepeatEnabled = false;

    std::bitset<KeyCount> keyStates;

    std::queue<Event> keyBuffer;
    std::queue<wchar_t> charBuffer;
};