#pragma once

class PanelWindowEditor final
{
public:
    PanelWindowEditor() = default;

    void Open() noexcept
    {
        active = true;
    }

    void Close() noexcept
    {
        active = false;
    }

    bool IsActive() const noexcept
    {
        return active;
    }

private:
    bool active = false;
};