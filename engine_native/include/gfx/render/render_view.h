#pragma once

namespace lumi::gfx::render
{
    struct Viewport
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };

    struct Scissor
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };

    struct RenderView
    {
        Viewport view;
        Scissor scissor;
    };
}