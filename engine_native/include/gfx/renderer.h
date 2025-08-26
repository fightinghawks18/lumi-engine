#pragma once

namespace lumi::gfx
{
    /**
     * \brief Handles rendering operations/management
     * \tparam D The Device type this renderer is tied to
     */
    template <typename D>
    class IRenderer
    {
    public: 
        /**
         * \brief Initializes the renderer context
         * 
         * \return true Initialization succeeded
         * \return false Initialization failed
         */
        virtual bool Init() = 0;

        /**
         * \brief Cleans up the renderer context
         * \warning Active resources tied to this renderer can be dangerous, clean up resources before cleaning up the renderer
         */
        virtual void Cleanup() = 0;
    };
}