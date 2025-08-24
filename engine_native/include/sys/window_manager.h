#pragma once

#include <memory>
#include <vector>

#include "window.h"

namespace lumi::sys
{
    using WinPtr = std::shared_ptr<Window>;
    using WinRef = std::weak_ptr<Window>;

    /**
     * \brief Manages the creation, lifetime, and destruction of windows while allowing easy access to them
     */
    class WindowManager
    {
    public:
        // Destructor
        ~WindowManager();

        /**
         * \brief Starts video service to allow windows to be created
         * \warning This must be called once and not while it has already started
         */
        static bool Init();

        WinRef CreateWindow(const WindowProperties& properties);

        /**
         * \brief Updates all windows attached to this manager
         */
        void Update();

        /**
         * \brief Destroys all windows then stops the video service to prevent any new windows from being created
         * \warning This must be called once and not while it hasn't already started
         */
        void Cleanup();
    private:
        std::vector<WinPtr> _windows;
    };
}
