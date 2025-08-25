#include <sys_c/window.h>
#include <sys/window.h>

C_API_FUNC(SYS_C_API, uint8_t, window_closing, Window* w)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    return static_cast<uint8_t>(win->Closing());
}

C_API_FUNC(SYS_C_API, void, window_warp, Window* w, int x, int y)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->Warp(x, y);
}

C_API_FUNC(SYS_C_API, void, window_warp_relative, Window* w, int x, int y)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->WarpRelative(x, y);
}

C_API_FUNC(SYS_C_API, void, window_resize, Window* win, int w, int h)
{
    auto window = reinterpret_cast<lumi::sys::Window*>(win);
    window->Resize(w, h);
}

C_API_FUNC(SYS_C_API, void, window_set_name, Window* w, char* name)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetName(std::string(name));
}

C_API_FUNC(SYS_C_API, void, window_set_icon, Window* w, char* icon)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetIcon(std::string(icon));
}

C_API_FUNC(SYS_C_API, void, window_set_size_limits, Window* w, 
    int wMin, int wMax, 
    int hMin, int hMax)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetSizeLimits(wMin, wMax, hMin, hMax);
}

C_API_FUNC(SYS_C_API, void, window_set_width_limits, Window* w, 
    int wMin, int wMax)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetWidthLimits(wMin, wMax);
}

C_API_FUNC(SYS_C_API, void, window_set_height_limits, Window* w,
    int hMin, int hMax)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetHeightLimits(hMin, hMax);
}

C_API_FUNC(SYS_C_API, void, window_set_resizable, Window* w, 
    uint8_t resizable)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetResizable(static_cast<bool>(resizable));
}

C_API_FUNC(SYS_C_API, void, window_set_bordered, Window* w,
    uint8_t bordered)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetBordered(static_cast<bool>(bordered));
}

C_API_FUNC(SYS_C_API, void, window_set_mode, Window* w,
    int mode)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    win->SetMode(static_cast<lumi::sys::WindowMode>(mode));
}

C_API_FUNC(SYS_C_API, void, window_destroy, Window* w)
{
    delete reinterpret_cast<lumi::sys::Window*>(w);
}

C_API_FUNC(SYS_C_API, uint8_t, window_is_valid, Window* w)
{
    auto win = reinterpret_cast<lumi::sys::Window*>(w);
    return static_cast<uint8_t>(win->IsWindow());
}