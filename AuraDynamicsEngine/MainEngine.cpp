#include "App.h"
#include "EngineException.h"
#include "MainWindow.h"
#include <objbase.h>

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, 
    int nShowCmd)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    try
    {
        
       return App{}.Go();
    }
    catch (const EngineException& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "Unknown exception", "Standard Exception", MB_OK | MB_ICONERROR);
    }
    return -1;
}
