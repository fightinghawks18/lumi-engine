#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <gfx/render/render_orchestrator.h>
#include <gfx/resources/image_buffer.h>

namespace lumi::gfx
{
    using render::RenderOrchestrator;
    using resources::IImageBuffer;

    class IRenderTarget
    {
    public:
        virtual bool Init(const uint32_t maxInFlight) = 0;
        virtual void Resize(const int width, const int height) = 0;
        virtual void StartRendering(const uint32_t index) = 0;
        virtual void EndRendering(const uint32_t index) = 0;
        virtual void SubmitRendering(const uint32_t index) = 0;
        virtual bool OutOfDate() const = 0;
        virtual void Cleanup() = 0;

        virtual int GetWidth() = 0;
        virtual int GetHeight() = 0;
        virtual std::shared_ptr<IImageBuffer> GetColorBuffer(const uint32_t index) = 0;
        virtual std::shared_ptr<IImageBuffer> GetDepthBuffer(const uint32_t index) = 0;
    };
}