#pragma once

#include "render_info.h"
#include <gfx/render_target.h>
#include <gfx/resources/image_buffer.h>

namespace lumi::gfx::render
{
    using resources::IImageBuffer;

    class IRenderContext
    {
    public:
        virtual ~IRenderContext() = default;
        void SetFrameNumber(const uint32_t frameNum) { _frameNum = frameNum; }
        virtual void SetRenderTarget(IRenderTarget& window) = 0;
        virtual void BeginRecording(const RenderInfo& info) = 0;
        virtual void EndRecording(const RenderInfo& info) = 0;

        [[nodiscard]] uint32_t GetFrameNumber() { return _frameNum; }
    protected:
        uint32_t _frameNum = 0;
    };
}