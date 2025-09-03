#include <gfx/render/render_context.h>
#include <d3d12.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace lumi::gfx::d3d12::render
{
    using gfx::render::IRenderContext;
    using gfx::render::RenderInfo;
    using gfx::render::RenderColorInfo;
    using gfx::render::RenderDepthInfo;
    using gfx::render::RenderLoadOp;
    using gfx::render::RenderStoreOp;
    using gfx::render::Viewport;
    using gfx::render::Scissor;
    

    class D3D12RenderContext : public IRenderContext
    {
    public:
        void SetRenderTarget(IRenderTarget& window) override;
        void BeginRecording(const RenderInfo& info) override;
        void EndRecording(const RenderInfo& info) override;
    private:
        ComPtr<ID3D12CommandAllocator> _commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> _commandList;
    };
}