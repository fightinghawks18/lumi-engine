#pragma once

#include <cstdint>
#include "gpu_resource.h"

namespace lumi::gfx::resources
{
    enum class ImageFormat
    {
        Undefined,
        RGBA8,
        RGBA16F,
        RGBA32F,
        Depth24Stencil8,
    };

    enum class ImageUsage : uint32_t
    {
        Undefined = 0,
        Render = 1 << 0,
        DepthStencil = 1 << 1,
        Shader = 1 << 2,
        UAV = 1 << 3
    };

    inline ImageUsage operator|(ImageUsage a, ImageUsage b) {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline ImageUsage operator&(ImageUsage a, ImageUsage b) {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    inline ImageUsage& operator|=(ImageUsage& a, ImageUsage b) {
        a = a | b;
        return a;
    }
    inline ImageUsage& operator&=(ImageUsage& a, ImageUsage b) {
        a = a & b;
        return a;
    }

    enum class ImageState
    {
        Undefined,
        Color,
        DepthStencil,
        Shader,
        UAV,
        Present
    };

    struct ImageDesc
    {
        uint32_t width;
        uint32_t height;
        ImageFormat format;
        ImageUsage usage;
    };

    class IImageBuffer : public IGpuResource
    {
    public:
        virtual ~IImageBuffer() = default;

        IImageBuffer& SetDesc(const ImageDesc& desc) { _description = desc; return *this; }
        IImageBuffer& SetWidth(const uint32_t width) { _description.width = width; return *this; }
        IImageBuffer& SetHeight(const uint32_t height) { _description.height = height; return *this; }
        IImageBuffer& SetFormat(const ImageFormat& format) { _description.format = format; return *this; }
        IImageBuffer& SetUsage(const ImageUsage& usage) { _description.usage = usage; return *this; }
        
        virtual bool Create() override = 0;

        /**
         * \brief Transitions the image's state to a new state
         * \note This is useful for reading/writing the image
         * \warning Do not attempt to transition to Undefined, it is used as an initial state
         * 
         * \param toState The new state that this image should attempt to transition to
         */
        virtual void Transition(const ImageState& toState) = 0;
        virtual void Destroy() override = 0;

        [[nodiscard]] uint32_t GetWidth() const { return _description.width; }
        [[nodiscard]] uint32_t GetHeight() const { return _description.height; }
        [[nodiscard]] ImageFormat GetFormat() const { return _description.format; }
        [[nodiscard]] ImageUsage GetUsage() const { return _description.usage; }
        [[nodiscard]] ImageState GetState() const { return _state; }
        [[nodiscard]] virtual void* Get() = 0;
    protected:
        ImageDesc _description;
        ImageState _state = ImageState::Undefined;
    };
}