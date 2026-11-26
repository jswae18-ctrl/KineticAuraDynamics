#include "Keyboard.h"

// =========================================================================================
// KEY STATE
// =========================================================================================
bool Keyboard::KeyIsPressed(unsigned char keyCode) const noexcept
{
    return keyStates[keyCode];
}

void Keyboard::ClearState() noexcept
{
    keyStates.reset();
}

// =========================================================================================
// KEY EVENT BUFFER
// =========================================================================================
std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
    if (keyBuffer.empty())
    {
        return std::nullopt;
    }

    Event event = keyBuffer.front();
    keyBuffer.pop();

    return event;
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return keyBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
    keyBuffer = {};
}

void Keyboard::OnKeyPressed(unsigned char keyCode) noexcept
{
    keyStates[keyCode] = true;

    keyBuffer.emplace(Event::Type::Press, keyCode);
    TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keyCode) noexcept
{
    keyStates[keyCode] = false;

    keyBuffer.emplace(Event::Type::Release, keyCode);
    TrimBuffer(keyBuffer);
}

// =========================================================================================
// CHARACTER BUFFER
// =========================================================================================
std::optional<wchar_t> Keyboard::ReadChar() noexcept
{
    if (charBuffer.empty())
    {
        return std::nullopt;
    }

    const wchar_t character = charBuffer.front();
    charBuffer.pop();

    return character;
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return charBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    charBuffer = {};
}

void Keyboard::OnChar(wchar_t character) noexcept
{
    charBuffer.emplace(character);
    TrimBuffer(charBuffer);
}

// =========================================================================================
// BUFFER CONTROL
// =========================================================================================
void Keyboard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

// =========================================================================================
// AUTOREPEAT CONTROL
// =========================================================================================
void Keyboard::EnableAutorepeat() noexcept
{
    autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
    autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
    return autorepeatEnabled;
}