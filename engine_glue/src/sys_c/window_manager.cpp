
#include <iostream>
#include <sys_c/window_manager.h>
#include <sys/window_manager.h>

C_API_FUNC(SYS_C_API, WindowManager*, window_manager_create, void)
{
    auto winManager = new lumi::sys::WindowManager();
    if (!winManager->Init())
    {
        std::cerr << "Failed to init window manager!" << std::endl;
        delete winManager;
        return nullptr;
    }
    return reinterpret_cast<WindowManager*>(winManager);
}

C_API_FUNC(SYS_C_API, Window *, window_manager_create_window, WindowManager* wm)
{
    auto winManager = reinterpret_cast<lumi::sys::WindowManager*>(wm);
    auto win = winManager->CreateWindow({
        "Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600
    });
    return reinterpret_cast<Window*>(win.get());
}


C_API_FUNC(SYS_C_API, void, window_manager_update, WindowManager* wm)
{
    auto winManager = reinterpret_cast<lumi::sys::WindowManager*>(wm);
    return winManager->Update();
}

C_API_FUNC(SYS_C_API, void, window_manager_destroy, WindowManager* wm)
{
    delete reinterpret_cast<lumi::sys::WindowManager*>(wm);
}