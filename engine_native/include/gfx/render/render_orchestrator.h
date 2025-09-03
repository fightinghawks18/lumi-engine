#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include "render_pass.h"

namespace lumi::gfx::render
{
    class RenderOrchestrator
    {
    using Pass = std::pair<std::string, RenderPass>;
    public:
        /**
         * \brief Adds a new render pass to this orchestrator
         * 
         * \param name The name of the render pass
         * \param info The render pass info to use
         */
        void NewPass(const std::string& name, RenderPass& info);

        /* Executes all passes in this orchestrator */
        void Execute(IRenderContext& ctx);

        [[nodiscard]] RenderPass* GetPass(const std::string& name);
    private:
        std::vector<Pass> _renderPasses;
        
    };
}