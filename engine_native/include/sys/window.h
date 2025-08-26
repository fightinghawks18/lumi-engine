#pragma once

#include <iostream>
#include <string>
#include <SDL3/SDL.h>

namespace lumi::sys
{
    /**
     * \brief WindowMode affects the appearance and behavior of a window
     * 
     */
    enum class WindowMode
    {
        Windowed, /* Window's position and scale is independent from display */
        Fullscreen, /* Window's position and scale are dependent of display */
        FullscreenBorderless /* Imitates Fullscreen but maintains the features from Windowed */
    };

    struct WindowProperties
    {
        std::string title;
        std::string icon;
        int x, y;
        int w, h;
        int wMin, wMax;
        int hMin, hMax;
        WindowMode mode;
        bool resizable;
        bool bordered;
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
         * \brief Warps the window to the provided position on the screen
         * 
         * \param x The horizontal position to warp this window to (in px)
         * \param y The vertical position to warp this window to (in px)
         */
        void Warp(const int x, const int y);

        /**
         * \brief Warps the window relative to the screen size to the provided position on the screen
         * \note For example, setting x and y to 0.5 will center the window on your screen
         * 
         * \param x The horizontal position to warp this window to
         * \param y The vertical position to warp this window to
         */
        void WarpRelative(const int x, const int y);

        /**
         * \brief Changes the window's size
         * \note If the size exceeds the set width/height limits, it'll be clamped to the min/max limits
         * 
         * \param w The width this window should have (in px)
         * \param h The height this window should have (in px)
         */
        void Resize(int w, int h);

        /**
         * \brief Changes the window's name
         * 
         * \param name The new name this window should use
         */
        void SetName(const std::string& name);

        /**
         * \brief Changes the window's icon
         * \warning The icon must be a *.bmp to load successfully, or it'll fail
         * 
         * \param icon The path to the icon/image file
         */
        void SetIcon(const std::string& icon);
        
        /**
         * \brief Sets the scaling limits of this window
         * \note To change width only, use SetWidthLimits()
         * \note To change height only, use SetHeightLimits()
         * 
         * \param wMin The minimum this window's width can scale to
         * \param wMax The maximum this window's width can scale to
         * \param hMin The minimum this window's height can scale to
         * \param hMax The maximum this window's height can scale to
         */
        void SetSizeLimits(int wMin, int wMax, int hMin, int hMax);

        /**
         * \brief Sets the limits of the window's width
         * \note 
         * 
         * \param min The minimum width this window can scale to (in px)
         * \param max The maximum width this window can scale to (in px)
         */
        void SetWidthLimits(int min, int max);
        
        /**
         * \brief Sets the limits of the window's height
         * \note 
         * 
         * \param min The minimum height this window can scale to (in px)
         * \param max The maximum height this window can scale to (in px)
         */
        void SetHeightLimits(int min, int max);

        /**
         * \brief Changes whether this window is allowed to be resized by the user's cursor
         * \note Even if resizable is false, you can still resize the window via Resize()
         * 
         * \param resizable Determines if resizing is allowed for this window
         */
        void SetResizable(const bool resizable);

        /**
         * \brief Changes whether this window should have a border
         * \note This is mainly used for Fullscreen Borderless
         * 
         * \param bordered Whether a border should be applied to the window
         */
        void SetBordered(const bool bordered);

        /**
         * \brief Changes the window's current mode
         * \note The window mode changes the appearance and behavior of a window
         * 
         * \param mode What mode the window should use
         */
        void SetMode(const WindowMode& mode);

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
         * \brief Gets the display's bounds that this window is currently on
         * 
         * \return SDL_Rect The display's bounds
         */
        [[nodiscard]] bool GetDisplayBounds(SDL_Rect* rect) 
        {
            SDL_DisplayID displayIdx = SDL_GetDisplayForWindow(_handle);
            if (displayIdx == 0)
            {
                std::cout << "Failed to get display index for window " << GetID() << "; " 
                          << "Falling back to primary display" << std::endl;
                displayIdx = SDL_GetPrimaryDisplay();
            }
            SDL_Rect displayBounds;
            if (!SDL_GetDisplayBounds(displayIdx, &displayBounds)) // Ensure that we can retrieve a rectangle of the display
            {
                std::cerr << "Failed to get display bounds for window " << GetID() 
                          << " "
                          << SDL_GetError() << std::endl;
                return false;
            }
            *rect = displayBounds;
            return true;
        }

        /**
         * \brief Checks if the window requested to close.
         * \return true - Window wants to close.
         * \return false - Window wants to stay active.
         */
        [[nodiscard]] bool Closing() const { return _needsClose; }

        [[nodiscard]] int GetWidth() const { return _width; }
        [[nodiscard]] int GetHeight() const { return _height; }

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

        WindowMode _mode = WindowMode::Windowed;
        bool _resizable = false;
        bool _bordered = false;
        
        std::string _title;
        std::string _icon;
        int _width = 0;
        int _height = 0;
        int _x = 0;
        int _y = 0;
        int _w = 0;
        int _h = 0;
        int _wMax = 0;
        int _hMax = 0;
        int _wMin = 0;
        int _hMin = 0;
    };
}