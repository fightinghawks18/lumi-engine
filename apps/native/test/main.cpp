#include <sys/window_manager.h>

int main()
{
    lumi::sys::WindowManager winManager;
    if (!winManager.Init())
    {
        return -1;
    }

    const auto win = winManager.CreateWindow({
        "Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600
    });

    while (true)
    {
        const auto window = win.lock();
        if (!window) // The window was destroyed
            break;

        if (window->Closing())
            break;

        winManager.Update();
    }

    winManager.Cleanup();
    return 0;
}