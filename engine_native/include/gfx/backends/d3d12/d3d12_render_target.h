#pragma once

#include <gfx/backends/d3d12/resources/d3d12_image_buffer.h>
#include <gfx/render_target.h>
#include <sys/window_manager.h>
#include <gfx/backends/d3d12/resources/d3d12_sync.h>
#include "d3d12_device.h"

namespace lumi::gfx::d3d12
{
    using gfx::render::RenderOrchestrator;
    using resources::D3D12ImageBuffer;
    using resources::ImageState;
    using resources::D3D12Sync;

    class D3D12RenderTarget : public IRenderTarget
    {
    public:
        D3D12RenderTarget(D3D12Device& device, sys::WinPtr& window);
        ~D3D12RenderTarget();

        bool Init(const uint32_t maxInFlight) override;
        void Resize(const int width, const int height) override;
        void StartRendering(const uint32_t index) override;
        void EndRendering(const uint32_t index) override;
        void SubmitRendering(const uint32_t index) override;
        bool OutOfDate() const override;
        void Cleanup() override;

        int GetWidth() override { return _window->GetWidth(); }
        int GetHeight() override { return _window->GetHeight(); }

        [[nodiscard]] ComPtr<ID3D12CommandAllocator> GetCommandAllocator(const uint32_t index) { return _commandAllocators[index]; }
        [[nodiscard]] ComPtr<ID3D12GraphicsCommandList> GetCommandList(const uint32_t index) { return _commandLists[index]; }
        [[nodiscard]] std::shared_ptr<IImageBuffer> GetColorBuffer(const uint32_t index) override { return _colorBuffers[index]; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVColorHandle(const uint32_t index) { return _colorBuffers[index]->GetRTVHandle(); }
        [[nodiscard]] std::shared_ptr<IImageBuffer> GetDepthBuffer(const uint32_t index) override { return _depthBuffers[index]; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDepthHandle(const uint32_t index) { return _depthBuffers[index]->GetRTVHandle(); }
    private:
        D3D12Device& _device;
        sys::WinPtr& _window;
        ComPtr<IDXGISwapChain1> _swapChain;
        std::vector<ComPtr<ID3D12CommandAllocator>> _commandAllocators;
        std::vector<ComPtr<ID3D12GraphicsCommandList>> _commandLists;
        std::vector<std::shared_ptr<D3D12ImageBuffer>> _colorBuffers;
        std::vector<std::shared_ptr<D3D12ImageBuffer>> _depthBuffers;
        std::shared_ptr<D3D12Sync> _sync;

        uint32_t _maxFramesInFlight = 0;

        bool CreateSync();
        bool CreateSwapChain();
        bool CreateImages();
        bool CreateCommandAllocator();
        bool CreateCommandList();

        void DestroyCommandList();
        void DestroyCommandAllocator();
        void DestroyImages();
        void DestroySwapChain();
        void DestroySync();
    };
}