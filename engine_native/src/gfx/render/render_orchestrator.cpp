#include <gfx/render/render_context.h>
#include <gfx/render/render_orchestrator.h>
#include <debugging/logger.h>

namespace lumi::gfx::render
{
    void RenderOrchestrator::NewPass(const std::string& name, RenderPass& info)
    {
        auto pass = GetPass(name);
        if (pass)
        {
            debugging::Logger::Instance().LogError(
                "Attempted to add a pass called {} but one was already added!",
                name
            );
            delete pass;
            return;
        }
        _renderPasses.push_back({name, info});
    }

    void RenderOrchestrator::Execute(IRenderContext& ctx)
    {
        for (auto& pass : _renderPasses)
        {
            for (const auto& target : pass.second.targets)
            {
                ctx.SetRenderTarget(*target);
                pass.second.execute(ctx, target);
            }
        }
    }

    RenderPass* RenderOrchestrator::GetPass(const std::string& name)
    {
        auto it = std::find_if(_renderPasses.begin(), _renderPasses.end(), 
            [&](Pass pass)
        {
            return pass.first == name;
        });
        if (it != _renderPasses.end())
        {
            return &(it->second);
        }
        return nullptr;
    }
}