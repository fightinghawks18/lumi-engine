#include <iostream>
#include <unordered_map>
#include <sys/window_manager.h>

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
            std::cerr << "Failed to start video service" << std::endl;
            return false;
        }
        return true;
    }

    WinPtr WindowManager::CreateWindow(const WindowProperties& properties)
    {
        auto win = std::make_shared<Window>();
        if (!win->Init(properties))
        {
            std::cerr << "Failed to create a new window " << SDL_GetError() << std::endl;
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