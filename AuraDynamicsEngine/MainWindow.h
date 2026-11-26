#pragma once
#include <windows.h>
#include <memory>
#include "Graphics.h"
#include "Mouse.h"
#include "Keyboard.h"

class MainWindow
{
public:

    MainWindow(int width, int height, const wchar_t* name);
    ~MainWindow();

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    MainWindow& operator=(MainWindow&&) = delete;

    HWND GetHandle() const noexcept { return windowHandle; }
    unsigned int GetWidth() const noexcept { return width; }
    unsigned int GetHeight() const noexcept { return height; }

    Graphics& GetGraphics() noexcept { return *graphics; }
    const Graphics& GetGraphics() const noexcept { return *graphics; }

    Mouse mouse;
    Keyboard kbd;

private:
    
    // Route Win32's static message callback into this MainWindow instance.
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;


    HWND windowHandle = nullptr;
    unsigned int width = 0u;
    unsigned int height = 0u;
    std::unique_ptr<Graphics> graphics;
};