#include "MainWindow.h"
#include <windowsx.h>
#include <stdexcept>

namespace
{
    constexpr const wchar_t* PrimaryWindowClassName = L"AuraDynamicsPrimaryTrackerEngine";
    
    unsigned int ValidateWindowDimension(int requestedDimension)
    {
        if (requestedDimension <= 0)
        {
            throw std::invalid_argument("Window dimensions must be greater than zero.");
        }
        
        return static_cast<unsigned int>(requestedDimension);
    }
}

MainWindow::MainWindow(int requestedWidth, int requestedHeight, const wchar_t* name)
    :
    width(ValidateWindowDimension(requestedWidth)),
    height(ValidateWindowDimension(requestedHeight))
{
    // =========================================================================================
    // WINDOW CLASS REGISTRATION
    // =========================================================================================
    
    // Describes the native window type before creating an individual HWND.
    WNDCLASSEXW windowClass = {};

    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = HandleMsgSetup;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = PrimaryWindowClassName;

    if (!RegisterClassExW(&windowClass))
    {
        const DWORD error = GetLastError();

        if (error != ERROR_CLASS_ALREADY_EXISTS)
        {
            throw std::runtime_error("Failed to register main engine viewport class.");
        }
    }
    
    // =========================================================================================
    // WINDOW SIZE CALCULATION
    // =========================================================================================

    // Expands the requested client area to include the non-client window frame.
    RECT windowRect =
    {
        0,
        0,
        requestedWidth,
        requestedHeight
    };

    if (!AdjustWindowRect(
        &windowRect,
        WS_OVERLAPPEDWINDOW,
        FALSE
    ))
    {
        throw std::runtime_error("Failed to calculate main engine window dimensions.");
    }
    
    // =========================================================================================
    // NATIVE WINDOW CREATION
    // =========================================================================================
    const int windowWidth = windowRect.right - windowRect.left;
    const int windowHeight = windowRect.bottom - windowRect.top;
    
    windowHandle = CreateWindowExW(
        0,
        PrimaryWindowClassName,
        name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowWidth,
        windowHeight,
        nullptr,
        nullptr,
        windowClass.hInstance,
        this
    );

    if (!windowHandle)
    {
        throw std::runtime_error(
            "Failed to create the main engine window."
        );
    }

    // =========================================================================================
    // GRAPHICS CONSTRUCTION
    // =========================================================================================
    
    // Bind graphics resources only after the native window exists.
    try
    {
        graphics = std::make_unique<Graphics>(
            windowHandle,
            width,
            height
        );
    }
    catch (...)
    {
        DestroyWindow(windowHandle);
        windowHandle = nullptr;
        throw;
    }
}

// =========================================================================================
// WINDOW DESTRUCTION
// =========================================================================================
MainWindow::~MainWindow()
{
    graphics.reset();

    if (windowHandle && IsWindow(windowHandle))
    {
        DestroyWindow(windowHandle);
    }

    windowHandle = nullptr;

    UnregisterClassW(
        PrimaryWindowClassName,
        GetModuleHandleW(nullptr)
    );
}

// =========================================================================================
// WIN32 MESSAGE PROCEDURE SETUP
// =========================================================================================
LRESULT CALLBACK MainWindow::HandleMsgSetup(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
) noexcept
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* const creationData =
            reinterpret_cast<CREATESTRUCTW*>(
                lParam
            );

        MainWindow* const window =
            static_cast<MainWindow*>(
                creationData->lpCreateParams
            );

        SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(
                window
            )
        );

        SetWindowLongPtrW(
            hWnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(
                &MainWindow::HandleMsgThunk
            )
        );

        window->windowHandle = hWnd;

        return window->HandleMsg(
            hWnd,
            msg,
            wParam,
            lParam
        );
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam
    );
}

LRESULT CALLBACK MainWindow::HandleMsgThunk(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
) noexcept
{
    MainWindow* const window =
        reinterpret_cast<MainWindow*>(
            GetWindowLongPtrW(
                hWnd,
                GWLP_USERDATA
            )
        );

    if (!window)
    {
        return DefWindowProcW(
            hWnd,
            msg,
            wParam,
            lParam
        );
    }

    return window->HandleMsg(
        hWnd,
        msg,
        wParam,
        lParam
    );
}

// =========================================================================================
// PRIMARY WINDOW MESSAGE PROCESSOR
// =========================================================================================

LRESULT MainWindow::HandleMsg(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
) noexcept
{
    switch (msg)
    {
        // =====================================================================
        // WINDOW LIFETIME
        // =====================================================================

        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        /* =====================================================================
         * INPUT FOCUS SAFETY
         * Prevent keys or mouse buttons from remaining logically pressed 
         * after the application loses focus. 
         * ===================================================================== */

        case WM_KILLFOCUS:
        {
            kbd.ClearState();

            if (mouse.leftIsPressed)
            {
                mouse.OnLeftReleased(mouse.x, mouse.y);
            }

            if (mouse.rightIsPressed)
            {
                mouse.OnRightReleased(mouse.x, mouse.y);
            }
            
            /* Synchronize the stored mouse position before processing later
             * movement messages. This prevents the first movement delta from
             * being measured from 0,0. */
            mouse.SynchronizePosition(mouse.x, mouse.y);

            return 0;
        }

        // =====================================================================
        // KEYBOARD INPUT
        // =====================================================================

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            const bool wasPreviouslyPressed = (lParam & 0x40000000) != 0;

            if (!wasPreviouslyPressed || kbd.AutorepeatIsEnabled())
            {
                kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            
            return 0;
        }

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
            return 0;
        }

        case WM_CHAR:
        {
            kbd.OnChar(static_cast<wchar_t>(wParam));
            return 0;
        }

        // =====================================================================
        // MOUSE MOVEMENT
        // =====================================================================

        case WM_MOUSEMOVE:
        {
            const int mouseX = GET_X_LPARAM(lParam);
            const int mouseY = GET_Y_LPARAM(lParam);

            if (!mouse.IsInWindow())
            {
                TRACKMOUSEEVENT tracking = {};

                tracking.cbSize = sizeof(tracking);

                tracking.dwFlags = TME_LEAVE;

                tracking.hwndTrack = hWnd;

                TrackMouseEvent(&tracking);

                mouse.OnMouseEnter();

                /* Synchronize the standalone mouse position registers 
                 * before processing later movement messages. This prevents 
                 * the first mouse movement from being measured from 0,0. */

                mouse.x = mouseX;
                mouse.y = mouseY;
                mouse.oldX = mouseX;
                mouse.oldY = mouseY;

                mouse.ClearDeltas();
            }
            else
            {
                mouse.OnMouseMove(mouseX, mouseY);
            }

            return 0;
        }

        case WM_MOUSELEAVE:
        {
            mouse.OnMouseLeave();
            mouse.ClearDeltas();

            return 0;
        }

        // =====================================================================
        // MOUSE BUTTON INPUT
        // =====================================================================
        
        case WM_LBUTTONDOWN:
        {
            const int mouseX = GET_X_LPARAM(lParam);
            const int mouseY = GET_Y_LPARAM(lParam);

            SetCapture(hWnd);

            mouse.OnLeftPressed(mouseX, mouseY);

            return 0;
        }

        case WM_LBUTTONUP:
        {
            const int mouseX = GET_X_LPARAM(lParam);
            const int mouseY = GET_Y_LPARAM(lParam);

            mouse.OnLeftReleased(mouseX, mouseY);

            if (!mouse.RightIsPressed())
            {
                ReleaseCapture();
            }
            
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            const int mouseX = GET_X_LPARAM(lParam);
            const int mouseY = GET_Y_LPARAM(lParam);

            SetCapture(hWnd);

            mouse.OnRightPressed(mouseX, mouseY);

            return 0;
        }

        case WM_RBUTTONUP:
        {
            const int mouseX = GET_X_LPARAM(lParam);
            const int mouseY = GET_Y_LPARAM(lParam);

            mouse.OnRightReleased(mouseX, mouseY);

            if (!mouse.LeftIsPressed())
            {
                ReleaseCapture();
            }

            return 0;
        }

        /* =====================================================================
         * MOUSE WHEEL INPUT
         * WM_MOUSEWHEEL provides screen coordinates. Convert the coordinates
         * into the same client coordinate space used by WM_MOUSEMOVE.
         * ===================================================================== */

        case WM_MOUSEWHEEL:
        {
            POINT mousePosition =
            {
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };

            ScreenToClient(hWnd, &mousePosition);

            mouse.OnWheelDelta(
                mousePosition.x,
                mousePosition.y,
                GET_WHEEL_DELTA_WPARAM(
                    wParam
                )
            );

            return 0;
        }
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam
    );
}