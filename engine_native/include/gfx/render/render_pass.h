#pragma once

#include <functional>
#include <vector>
#include <gfx/resources/gpu_resource.h>

#include "render_info.h"

namespace lumi::gfx
{
    class IRenderTarget;
}

namespace lumi::gfx::render
{
    class IRenderContext;
}

namespace lumi::gfx::render
{
    using resources::IGpuResource;

    struct RenderPass
    {
        std::vector<IRenderTarget*> targets;
        std::function<void(IRenderContext& ctx, IRenderTarget* target)> execute;
    };
}