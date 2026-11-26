#include "App.h"
#include <DirectXMath.h>
#include <chrono>
#include "Graphics.h"

// =========================================================================================
// APPLICATION CONSTRUCTION
// =========================================================================================
App::App()
    :
    diagnostics(R"(AuraSupportLab\Logs\aura-engine.log)"),
    earthOrbit(12.0f, 0.010f)
{
    // =====================================================================================
    // APPLICATION DEPENDENCIES
    // =====================================================================================
    wnd = std::make_unique<MainWindow>(WindowWidth, WindowHeight, L"Kinetic Aura Dynamics");
    
    Graphics& graphics = wnd->GetGraphics();
    ID3D11Device* const device = graphics.GetDevice();
    ID3D11DeviceContext* const deviceContext = graphics.GetContext();
    
    // =====================================================================================
    // SCENE CONSTRUCTION
    // =====================================================================================
    earth = std::make_unique<Earth>(device, deviceContext);
    sun = std::make_unique<Sun>(device, deviceContext);
    space = std::make_unique<Space>(device, deviceContext);

    
    // =====================================================================================
    // HUD COMPOSITION
    // =====================================================================================
    
    // Provides non-owning access to application systems required by HUD panels.
    HudPanelContext hudPanelContext;
    
    hudPanelContext.Add(camera);
    hudPanelContext.Add(*earth);
    hudPanelContext.Add(*sun);
    hudPanelContext.Add(*space);
    hudPanelContext.Add(earthOrbit);
    
    // Construct the HUD only after every registered panel dependency exists.
    hud = std::make_unique<HUD>(
        device,
        deviceContext,
        hudPanelContext
    );
    
    
    // =====================================================================================
    // INITIAL SCENE STATE
    // =====================================================================================
    const DirectX::XMFLOAT3 initialSunPosition = {10.0f, 4.0f, 8.0f};
    const DirectX::XMFLOAT3 initialEarthOrbitOffset = earthOrbit.GetPosition();
    const DirectX::XMFLOAT3 initialEarthPosition =
    {
        initialSunPosition.x + initialEarthOrbitOffset.x,
        initialSunPosition.y + initialEarthOrbitOffset.y,
        initialSunPosition.z + initialEarthOrbitOffset.z
    };
    
    const DirectX::XMVECTOR initialEarthPositionVector =
    DirectX::XMLoadFloat3(
        &initialEarthPosition
    );

    const DirectX::XMVECTOR initialSunPositionVector =
        DirectX::XMLoadFloat3(
            &initialSunPosition
        );

    DirectX::XMVECTOR earthToSunDirection =
        DirectX::XMVectorSubtract(
            initialSunPositionVector,
            initialEarthPositionVector
        );

    earthToSunDirection =
        DirectX::XMVector3Normalize(
            earthToSunDirection
        );

    const DirectX::XMVECTOR initialCameraPositionVector =
        DirectX::XMVectorAdd(
            initialEarthPositionVector,
            DirectX::XMVectorScale(
                earthToSunDirection,
                4.0f
            )
        );

    DirectX::XMFLOAT3 initialCameraPosition = {};

    DirectX::XMStoreFloat3(
        &initialCameraPosition,
        initialCameraPositionVector
    );
    
    camera.SetInitialPose(initialCameraPosition, 0.0f, 0.0f);
    
    // Starts the engine focused on Earth.
    camera.FocusOn(initialEarthPosition);
    
    // =====================================================================================
    // APPLICATION STARTUP
    // =====================================================================================
    ShowWindow(wnd->GetHandle(), SW_SHOW);
    UpdateWindow(wnd->GetHandle());
    
    
    // Reports successful startup after all application systems are ready.
    diagnostics.GetDiagnostics().Report(
        AuraDiagnosticEvent(
            AuraID("engine.startup"),
            AuraID("aura.dynamics.engine"),
            AuraDiagnosticSeverity::Info,
            "AuraDynamicsEngine started successfully."
        )
    );
}


// =========================================================================================
// MAIN APPLICATION LOOP
// =========================================================================================
int App::Go()
{
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }

        RECT clientRect = {};
        GetClientRect(wnd->GetHandle(), &clientRect);

        const unsigned int activeWidth = static_cast<unsigned int>(clientRect.right - clientRect.left);
        const unsigned int activeHeight = static_cast<unsigned int>(clientRect.bottom - clientRect.top);

        if (activeWidth == 0u || activeHeight == 0u)
        {
            continue;
        }

        if (activeWidth != currentWidth || activeHeight != currentHeight)
        {
            currentWidth = activeWidth;
            currentHeight = activeHeight;
            wnd->GetGraphics().Resize(currentWidth, currentHeight);
        }

        DoFrame();
    }

    return static_cast<int>(msg.wParam);
}

// =========================================================================================
// FRAME PROCESSING
// =========================================================================================
void App::DoFrame()
{
    // -------------------------------------------------------------------------
    // FRAME TIMING
    // -------------------------------------------------------------------------
    using Clock = std::chrono::steady_clock;
    static Clock::time_point previousFrameTime = Clock::now();
    const Clock::time_point currentFrameTime = Clock::now();

    float deltaTime = std::chrono::duration<float>(currentFrameTime - previousFrameTime).count();
    previousFrameTime = currentFrameTime;

    if (deltaTime > 0.1f)
    {
        deltaTime = 0.1f;
    }

    wnd->GetGraphics().ClearBuffers(0.0f, 0.0f, 0.0f);

    const float earthRotationSpeed = 0.025f;
    const float sunRotationSpeed = 0.0f;
    const DirectX::XMFLOAT3 sunPosition = { 10.0f, 4.0f, 8.0f };
    const float sunScale = 2.5f;

    // ---------------------------------------------------------
    // SCENE UPDATE
    // ---------------------------------------------------------
    earthRotation += earthRotationSpeed * deltaTime;
    sunRotation += sunRotationSpeed * deltaTime;

    earthOrbit.Update(deltaTime);

    if (earth != nullptr)
    {
        earth->Update(deltaTime);
    }

    const DirectX::XMFLOAT3 earthOrbitOffset = earthOrbit.GetPosition();
    const DirectX::XMFLOAT3 earthPosition = 
    {
        sunPosition.x + earthOrbitOffset.x,
        sunPosition.y + earthOrbitOffset.y,
        sunPosition.z + earthOrbitOffset.z
    };

    // ---------------------------------------------------------
    // APPLICATION INPUT
    // ---------------------------------------------------------
    camera.SetFocusTarget(earthPosition);
    
    const bool speedKeyIsPressed = wnd->kbd.KeyIsPressed(VK_TAB);
    
    if (speedKeyIsPressed && !speedKeyWasPressed)
    {
        bool panelWasRestored = false;
        
        if (hud != nullptr)
        {
            panelWasRestored = hud->ShowSpeedPanel();
        }
        
        if (!panelWasRestored && camera.IsFreeFlight())
        {
            if (camera.IsEnteringSpeed())
            {
                camera.CancelSpeedEntry();
            }
            else
            {
                wnd->kbd.FlushChar();

                camera.BeginSpeedEntry();
            }
        }
    }
    
    speedKeyWasPressed = speedKeyIsPressed;
    
    const bool freeFlightKeyIsPressed = wnd->kbd.KeyIsPressed('1');
    const bool focusKeyIsPressed = wnd->kbd.KeyIsPressed('2');

    if (!camera.IsEnteringSpeed())
    {
        if (freeFlightKeyIsPressed && !freeFlightKeyWasPressed)
        {
            camera.SetFreeFlight();
        }

        if (focusKeyIsPressed && !focusKeyWasPressed)
        {
            camera.FocusOn(earthPosition);
        }
    }

    freeFlightKeyWasPressed = freeFlightKeyIsPressed;
    focusKeyWasPressed = focusKeyIsPressed;

    // ---------------------------------------------------------
    // HUD INPUT AND UPDATE
    // ---------------------------------------------------------
    if (hud != nullptr)
    {
        while (auto event = wnd->mouse.Read())
        {
            hud->HandleMouseEvent(*event);
        }

        hud->Update(deltaTime);
    }
    
    // ----------------------------------------------------------
    // CAMERA UPDATE
    // ----------------------------------------------------------
    bool editorLauncherIsOpen = false;
    bool allowCameraMouseInput = true;

    if (hud != nullptr)
    {
        editorLauncherIsOpen =
            hud->IsEditorLauncherOpen();

        allowCameraMouseInput =
            !hud->WantsMouseCapture(
                wnd->mouse
            );
    }

    if (!editorLauncherIsOpen ||
        !camera.IsFreeFlight())
    {
        camera.Update(
            deltaTime,
            wnd->kbd,
            wnd->mouse,
            allowCameraMouseInput &&
            !editorLauncherIsOpen
        );
    }
    // ---------------------------------------------------------
    // FRAME TRANSFORMS
    // ---------------------------------------------------------
    const float earthStartingLongitude = DirectX::XMConvertToRadians(-90.0f);
    const DirectX::XMMATRIX earthWorld = DirectX::XMMatrixRotationY(earthStartingLongitude + earthRotation) *
                                         DirectX::XMMatrixTranslation(earthPosition.x, earthPosition.y, earthPosition.z);
    
    const DirectX::XMMATRIX sunWorld = DirectX::XMMatrixScaling(sunScale, sunScale, sunScale) *
                                       DirectX::XMMatrixRotationY(sunRotation) *
                                       DirectX::XMMatrixTranslation(sunPosition.x, sunPosition.y, sunPosition.z);

    const DirectX::XMMATRIX mView = camera.GetViewMatrix();
    const DirectX::XMFLOAT3& cameraPosition = camera.GetPosition();
    
    const float aspect = static_cast<float>(currentWidth) / static_cast<float>(currentHeight);
    const DirectX::XMMATRIX mProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, aspect, 0.1f, 100.0f);

    const DirectX::XMMATRIX earthWVP = earthWorld * mView * mProjection;
    const DirectX::XMMATRIX sunWVP = sunWorld * mView * mProjection;
    
    // ---------------------------------------------------------
    // FRAME RENDERING
    // ---------------------------------------------------------
    if (space != nullptr)
    {
        space->Draw(wnd->GetGraphics().GetContext(), mView, mProjection, cameraPosition);
    }
    
    if (earth != nullptr)
    {
        earth->Draw(wnd->GetGraphics().GetContext(), earthWVP, earthWorld, cameraPosition, sunPosition);
    }
    
    if (sun != nullptr)
    {
        sun->Draw(wnd->GetGraphics().GetContext(), sunWVP, sunWorld, cameraPosition);
    }
    
    if (hud != nullptr)
    {
        hud->Draw(currentWidth, currentHeight);
    }
    
    // -------------------------------------------------------------------------
    // FRAME COMPLETION
    // -------------------------------------------------------------------------
    
    // Consume mouse movement only after Camera has processed it.
    wnd->mouse.ClearDeltas();

    wnd->GetGraphics().Present();
}
