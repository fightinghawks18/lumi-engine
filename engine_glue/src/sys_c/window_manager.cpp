
#include <sys_c/window_manager.h>
#include <sys/window_manager.h>

C_API_FUNC(SYS_C_API, WindowManager *, window_manager_create, void)
{
    return reinterpret_cast<WindowManager*>(new lumi::sys::WindowManager());
}

C_API_FUNC(SYS_C_API, void, window_manager_destroy, WindowManager* wm)
{
    delete reinterpret_cast<lumi::sys::WindowManager*>(wm);
}