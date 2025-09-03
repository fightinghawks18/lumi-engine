#pragma once

#include <array>
#include <vector>
#include <string>
#include <gfx/resources/image_buffer.h>
#include "render_view.h"

namespace lumi::gfx::render
{
    using resources::IImageBuffer;

    /* Describes how info should be loaded */
    enum class RenderLoadOp
    {
        Clear, /* Discards all color/depth info from an image present and replaces it */
        Load, /* Loads previous color/depth info from image */
        DontCare /* Doesn't clear nor load color/depth from image */
    };

    /* Describes how info should be stored */
    enum class RenderStoreOp
    {
        Store, /* Saves color/depth info to image */
        DontCare /* Doesn't clear nor load color/depth from image */
    };

    struct RenderAttachmentInfo
    {
        IImageBuffer* image;
        RenderLoadOp loadOp = RenderLoadOp::Clear;
        RenderStoreOp storeOp = RenderStoreOp::Store;
    };

    struct RenderDepthInfo : RenderAttachmentInfo
    {
        /* What depth and stencil should we use if clearing */
        std::array<float, 2> depthStencil;
    };

    struct RenderColorInfo : RenderAttachmentInfo
    {
        /* What color we should use if clearing */
        std::array<float, 4> color;
    };

    /* Provides information when starting a new render */
    struct RenderInfo
    {
        /* Describes how info should be transformed */
        RenderView view;
        std::vector<RenderColorInfo> color;
        RenderDepthInfo* depth = nullptr;
    };
}