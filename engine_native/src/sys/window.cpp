#include <algorithm>
#include <sys/window.h>
#include <debugging/logger.h>

namespace lumi::sys
{
    bool Window::Start()
    {
        if (SDL_WasInit(SDL_INIT_VIDEO))
        {
            debugging::Logger::Instance().LogWarn("SDL was already initialized, cannot start");
            return false;
        }

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            debugging::Logger::Instance().LogError("Failed to initialize SDL: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    bool Window::Stop()
    {
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            debugging::Logger::Instance().LogWarn("SDL isn't initialized, cannot close");
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
            debugging::Logger::Instance().LogError("SDL must be initialized before creating windows");
            return false;
        }

        // Ensure window wasn't already created
        if (_handle)
        {
            debugging::Logger::Instance().LogWarn("Window {} has already been created", GetID());
            return false;
        }

        // Set window creation properties first
        _title = properties.title;
        _width = properties.w;
        _height = properties.h;

        // Create window
        _handle = CreateWindowObject();

        // Ensure window creation was successful
        if (!_handle)
        {
            debugging::Logger::Instance().LogError("Window creation failed: {}", SDL_GetError());
            return false;
        }

        // Assign properties that aren't included in SDL_CreateWindow
        Warp(properties.x, properties.y);
        SetBordered(properties.bordered);
        SetResizable(properties.resizable);
        SetSizeLimits(properties.wMin, properties.wMax, properties.hMin, properties.hMax);
        SetMode(properties.mode);
        SetIcon(properties.icon);

        return true;
    }

    void Window::Process(const SDL_Event& event)
    {
        // Run window only events if this event belongs to the window
        if (GetID() != event.window.windowID)
        {
            return;
        }

        // Run window-specific events
        switch (event.type)
        {
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            {
                if (_mode != WindowMode::FullscreenBorderless)
                {
                    break;
                }

                SDL_Rect displayBounds;
                if (!GetDisplayBounds(&displayBounds))
                {
                    break;
                }
                SDL_SetWindowPosition(_handle, displayBounds.x, displayBounds.y);
                SDL_SetWindowSize(_handle, displayBounds.w, displayBounds.h);
                break;
            }
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                _needsClose = true;
                break;
            }
        }
    }

    void Window::Warp(const int x, const int y)
    {
        _x = x;
        _y = y;

        if (_mode != WindowMode::Windowed) return;
        SDL_SetWindowPosition(_handle, _x, _y);
    }

    void Window::WarpRelative(const int x, const int y)
    {
        // Get display that the window is currently on
        SDL_Rect displayBounds;
        if (!GetDisplayBounds(&displayBounds))
        {
            return;
        }
        
        int screenW = displayBounds.w;
        int screenH = displayBounds.h;

        // Move window relative to the display's scale
        Warp(
            screenW * x, 
            screenH * y
        );
    }

    void Window::Resize(int w, int h)
    {
        w = std::clamp(w, _wMin, _wMax);
        h = std::clamp(h, _hMin, _hMax);

        _w = w;
        _h = h;

        if (_mode != WindowMode::Windowed) return;
        SDL_SetWindowSize(_handle, w, h);
    }

    void Window::SetName(const std::string& name)
    {
        _title = name;
        SDL_SetWindowTitle(_handle, _title.c_str());
    }

    void Window::SetIcon(const std::string& icon)
    {
        // Load image path as a surface
        SDL_Surface* surface = SDL_LoadBMP(icon.c_str());
        if (!surface)
        {
            debugging::Logger::Instance().LogError("Window {} failed to load window icon: {}", GetID(), SDL_GetError());
            return;
        }

        SDL_SetWindowIcon(_handle, surface);
        SDL_DestroySurface(surface);
        _icon = icon;
    }

    void Window::SetSizeLimits(int wMin, int wMax, int hMin, int hMax)
    {
        // Handle width
        wMax = std::clamp(wMax, wMin, wMax); // Can't be below min

        // Handle height
        hMax = std::clamp(hMax, hMin, hMax); // Can't be below min

        _hMin = hMin;
        _wMin = wMin;
        _hMax = hMax;
        _wMax = wMax;
        if (_mode != WindowMode::Windowed) return;
        SDL_SetWindowMinimumSize(_handle, _wMin, _hMin);
        SDL_SetWindowMaximumSize(_handle, _wMax, _hMax);
    }

    void Window::SetWidthLimits(int min, int max)
    {
        SetSizeLimits(min, max, _hMin, _hMax);
    }

    void Window::SetHeightLimits(int min, int max)
    {
        SetSizeLimits(_wMin, _wMax, min, max);
    }

    void Window::SetResizable(const bool resizable)
    {
        _resizable = resizable;
        if (_mode != WindowMode::Windowed) return;
        SDL_SetWindowResizable(_handle, _resizable);
    }

    void Window::SetBordered(const bool bordered)
    {
        _bordered = bordered;
        if (_mode != WindowMode::Windowed) return;
        SDL_SetWindowBordered(_handle, _bordered);
    }

    void Window::SetMode(const WindowMode& mode)
    {
        switch (mode)
        {
            case WindowMode::Windowed:
            {
                if (_mode == WindowMode::Fullscreen)
                {
                    SDL_SetWindowFullscreen(_handle, false);
                }

                // Reinstate window properties
                SetBordered(_bordered);
                SetResizable(_resizable);
                Warp(_x, _y);
                Resize(_w, _h);
                SetSizeLimits(_wMin, _wMax, _hMin, _hMax);
                break;
            }
            case WindowMode::Fullscreen:
            {
                SDL_SetWindowFullscreen(_handle, true);
                break;
            }
            case WindowMode::FullscreenBorderless:
            {
                SDL_Rect displayBounds;
                if (!GetDisplayBounds(&displayBounds))
                {
                    break;
                }

                if (_mode == WindowMode::Fullscreen)
                {
                    SDL_SetWindowFullscreen(_handle, false);
                }

                // Remove size limits
                SDL_SetWindowMinimumSize(_handle, 0, 0);
                SDL_SetWindowMaximumSize(_handle, 0, 0);
                
                // Remove border and resizing properties
                SDL_SetWindowBordered(_handle, false);
                SDL_SetWindowResizable(_handle, false);

                // Move window to top-left and resize to cover screen
                SDL_SetWindowPosition(_handle, displayBounds.x, displayBounds.y);
                SDL_SetWindowSize(_handle, displayBounds.w, displayBounds.h);
                break;
            }
        }
        _mode = mode;
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
