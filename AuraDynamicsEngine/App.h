#pragma once
#include <memory>
#include "MainWindow.h"
#include "Camera/Camera.h"
#include "Assets/EarthMesh/Earth.h"
#include "Assets/SunMesh/Sun.h"
#include "Assets/PlanetOrbit/PlanetOrbit.h"
#include "Background/Space.h"
#include "UI/HUD.h"
#include "Diagnostics/AuraDynamicsDiagnostics.h"

class App
{
public:
    
    static constexpr unsigned int WindowWidth  = 1280;
    static constexpr unsigned int WindowHeight = 720;

    App();
    ~App() = default;
    
    int Go();

private:
    void DoFrame();
    
    AuraDynamicsDiagnostics diagnostics;
    std::unique_ptr<MainWindow> wnd;
    std::unique_ptr<Earth> earth;
    std::unique_ptr<Sun> sun;
    std::unique_ptr<Space> space;
    PlanetOrbit earthOrbit;
    
    // Last client size applied to the graphics swap chain.
    unsigned int currentWidth = WindowWidth;
    unsigned int currentHeight = WindowHeight;
    
    Camera camera;
    std::unique_ptr<HUD> hud;
    
    // Previous key states used to detect single-press transitions.
    bool speedKeyWasPressed = false;
    bool freeFlightKeyWasPressed = false;
    bool focusKeyWasPressed = false;
    
    
    float earthRotation = 0.0f;
    float sunRotation = 0.0f;
};
