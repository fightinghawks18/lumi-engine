#include <sys_c/window.h>
#include <sys/window.h>

C_API_FUNC(SYS_C_API, Window*, window_create, void)
{
    auto win = new lumi::sys::Window();
    if (!win->Init({
        "Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600
    }))
    {
        delete win;
    }
    return reinterpret_cast<Window*>(win);
}

C_API_FUNC(SYS_C_API, void, window_destroy, Window* w)
{
    delete reinterpret_cast<lumi::sys::Window*>(w);
}