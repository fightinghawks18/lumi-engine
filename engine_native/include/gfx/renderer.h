#pragma once

#include <vector>
#include "render_target.h"

namespace lumi::gfx
{
    struct RenderOrchestration
    {
        std::vector<std::pair<IRenderTarget*, RenderOrchestrator>> orchestrators;
    };

    /**
     * \brief Handles rendering operations/management
     */
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

        virtual RenderOrchestration& StartRendering() = 0;
        virtual void EndRendering(RenderOrchestration& orchestration) = 0;

        /**
         * \brief Cleans up the renderer context
         * \warning Active resources tied to this renderer can be dangerous, clean up resources before cleaning up the renderer
         */
        virtual void Cleanup() = 0;
    };
}