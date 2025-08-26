#pragma once

namespace lumi::gfx
{
    /* Core context object for rendering */
    class IDevice
    {
    public:
        /**
         * \brief Initializes the rendering device
         * \note The rendering device supplies all the core objects that allow rendering to happen
         * \return true Initialization succeeded
         * \return false Initialization failed
         */
        virtual bool Init() = 0;

        /**
         * \brief Cleans up the rendering device 
         * \warning Active resources tied to this device can be dangerous, clean up resources before cleaning up the device
         */
        virtual void Cleanup() = 0;
    };
}