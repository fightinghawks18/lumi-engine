
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

C_API_FUNC(SYS_C_API, Window*, window_manager_create_window, WindowManager* wm, WindowProperties props)
{
    auto winManager = reinterpret_cast<lumi::sys::WindowManager*>(wm);
    
    lumi::sys::WindowProperties cppProps{};
    cppProps.title = props.title;
    cppProps.icon = props.icon;
    cppProps.x = props.x;
    cppProps.y = props.y;
    cppProps.w = props.w;
    cppProps.h = props.h;
    cppProps.wMin = props.wMin;
    cppProps.wMax = props.wMax;
    cppProps.hMin = props.hMin;
    cppProps.hMax = props.hMax;
    cppProps.mode = static_cast<lumi::sys::WindowMode>(props.mode);
    cppProps.resizable = props.resizable != 0;
    cppProps.bordered = props.bordered != 0;

    auto win = winManager->NewWindow(cppProps);
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