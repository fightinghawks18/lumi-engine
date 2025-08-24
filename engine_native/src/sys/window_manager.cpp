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

    WinRef WindowManager::CreateWindow(const WindowProperties& properties)
    {
        auto win = std::make_shared<Window>();
        if (!win->Init(properties))
        {
            std::cerr << "Failed to create a new window " << SDL_GetError() << std::endl;
            win.reset();
            return std::weak_ptr<Window>{};
        }
        _windows.push_back(win);
        return win;
    }

    void WindowManager::Update()
    {
        std::unordered_map<SDL_Window*, WinPtr> idToWindowMap;
        for (const WinPtr& win : _windows)
        {
            idToWindowMap[win->GetHandle()] = win;
        }

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            SDL_Window* winFromEvent = SDL_GetWindowFromEvent(&e);
            if (!winFromEvent) continue;

            const WinPtr win = idToWindowMap[winFromEvent];
            win->Process(e);
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
