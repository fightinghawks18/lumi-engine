#include <iostream>
#include <sys/window.h>

namespace lumi::sys
{
    bool Window::Start()
    {
        if (SDL_WasInit(SDL_INIT_VIDEO))
        {
            std::cout << "SDL has already been initialized!" << std::endl;
            return false;
        }

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        }
        return true;
    }

    bool Window::Stop()
    {
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            std::cout << "SDL isn't initialized!" << std::endl;
            return false;
        }

        SDL_Quit();
        return true;
    }

    Window::~Window()
    {
        Destroy();
    }

    bool Window::Init(const WindowProperties& properties)
    {
        // Ensure SDL is initialized before creating a window
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            std::cerr << "You must initialize SDL before creating windows" << std::endl;
            return false;
        }

        // Ensure window wasn't already created
        if (_handle)
        {
            std::cerr << "This window was already created! ID: " << GetID() << std::endl;
            return false;
        }

        // Set window creation properties first
        _title = properties.title;
        _x = properties.x;
        _y = properties.y;
        _width = properties.w;
        _height = properties.h;

        // Create window
        _handle = CreateWindowObject();

        // Ensure window creation was successful
        if (!_handle)
        {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }

        // Assign properties that aren't included in SDL_CreateWindow
        SDL_SetWindowPosition(_handle, _x, _y);

        return true;
    }

    void Window::Process(const SDL_Event& event)
    {
        // Ensure that this event belongs to this window
        if (GetID() != event.window.windowID)
        {
            std::cerr << "Event given with ID " << event.window.windowID
                      << "doesn't match this window's " << GetID() << std::endl;
            return;
        }

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            _needsClose = true;
        }
    }

    void Window::Destroy()
    {
        if (_handle)
        {
            SDL_DestroyWindow(_handle);
            _handle = nullptr;
        }
    }

    SDL_Window* Window::CreateWindowObject()
    {
        return SDL_CreateWindow
        (
            _title.c_str(),
            _width,
            _height,
            0
        );
    }
}
