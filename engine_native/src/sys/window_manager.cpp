#include <iostream>
#include <unordered_map>
#include <sys/window_manager.h>
#include <debugging/logger.h>

namespace lumi::sys
{
    WindowManager::~WindowManager()
    {
        Cleanup();
    }

    bool WindowManager::Init()
    {
        if (!Window::Start())
        {
            debugging::Logger::Instance().LogError("Failed to start window service: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    WinPtr WindowManager::NewWindow(const WindowProperties& properties)
    {
        auto win = std::make_shared<Window>();
        if (!win->Init(properties))
        {
            debugging::Logger::Instance().LogError("Failed to create window: {}", SDL_GetError());
            win.reset();
            return nullptr;
        }
        _windows.push_back(win);
        return win;
    }

    void WindowManager::Update()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            for (const auto& win : _windows)
            {
                win->Process(e);
            }
        }
    }

    void WindowManager::Cleanup()
    {
        for (WinPtr& win : _windows)
        {
            win.reset();
        }
        _windows.clear();
    }
}