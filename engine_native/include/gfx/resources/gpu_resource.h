#pragma once

namespace lumi::gfx::resources
{
    class IGpuResource
    {
    public:
        virtual bool Create() = 0;
        virtual void Destroy() = 0;
    };
}