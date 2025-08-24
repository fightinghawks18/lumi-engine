#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace lumi::sys
{
    struct WindowProperties
    {
        std::string title;
        int x, y;
        int w, h;
    };

    /**
     * \brief Represents a window managed by the user's system.
     * \details Manages the creation, lifetime, and destruction of the window.
     */
    class Window
    {
    public:
        /**
         * \brief Starts SDL3 window service.
         * \warning This should only be called once.
         */
        static bool Start();

        /**
         * \brief Stops SDL3 window service.
         * \warning This should only be called once, only works if it already started.
         */
        static bool Stop();

        // Destructor
        virtual ~Window();

        /**
         * \brief Creates this window's handle allowing it to be visible on screen.
         * 
         * \return true - Window creation succeeded.
         * \return false - Window creation failed.
         */
        [[nodiscard]] bool Init(const WindowProperties& properties);

        /**
         * \brief Processes all available window events (e.g. resizing, moving).
         * \param event The event to process this window with.
         * \warning You must ensure that event has the same windowID as this window, or else it won't apply to it.
         */
        void Process(const SDL_Event& event);

        /**
         * \brief Destroys this window's handle.
         * \warning The window must have a valid handle, check it with \code IsWindow()\endcode.
         */
        void Destroy();

        /**
         * \brief Checks if the window's handle is current active.
         * 
         * \return true - Handle is active.
         * \return false - Handle was destroyed.
         */
        [[nodiscard]] bool IsWindow() const { return _handle != nullptr; }

        /**
         * \brief Checks if the window requested to close.
         * \return true - Window wants to close.
         * \return false - Window wants to stay active.
         */
        [[nodiscard]] bool Closing() const { return _needsClose; }

        /**
         * \brief Retrieves the handle.
         * \return SDL_Window - The handle associated with this window.
         */
        [[nodiscard]] SDL_Window* GetHandle() const { return _handle; }

        /**
         * \brief Retrieves this window's windowID
         * \return SDL_WindowID - The windowID associated with this window
         */
        [[nodiscard]] SDL_WindowID GetID() const { return SDL_GetWindowID(_handle); }
    protected:
        /**
         * \brief Creates a new window object
         * \note Use this for different classes if you want to handle window creation that is different from the base Window class
         * \return SDL_Window
         */
        virtual SDL_Window* CreateWindowObject();
    private:
        SDL_Window* _handle = nullptr;
        bool _needsClose = false;

        std::string _title;
        int _width = 0;
        int _height = 0;
        int _x = 0;
        int _y = 0;
    };
}