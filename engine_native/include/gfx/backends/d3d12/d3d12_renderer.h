#pragma once

#include <gfx/renderer.h>
#include "d3d12_device.h"

#include <vector>
#include <sys/window_manager.h>

namespace lumi::gfx::d3d12
{
    struct D3D12RenderTarget
    {
        sys::WinPtr window;
        ComPtr<IDXGISwapChain4> swapChain;
        std::vector<ComPtr<ID3D12Resource>> renderTargets;
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT rtvDescriptorSize = 0;
        UINT frameIndex = 0;

        bool OutOfDate()
        {
            DXGI_SWAP_CHAIN_DESC desc = {};
            swapChain->GetDesc(&desc);

            return static_cast<int>(desc.BufferDesc.Width) != window->GetWidth() 
                   || static_cast<int>(desc.BufferDesc.Height) != window->GetHeight();
        }
    };

    class D3D12Renderer : public IRenderer<D3D12Device>
    {
    public:
        D3D12Renderer(D3D12Device& device);

        bool Init() override;

        D3D12RenderTarget AddRenderTarget(const sys::WinPtr& window);
        void RemoveRenderTarget(const sys::WinPtr& window);
        void ResizeRenderTarget(D3D12RenderTarget& target, UINT width, UINT height);

        void StartRender(D3D12RenderTarget& target);
        void EndRender(D3D12RenderTarget& target);

        void Cleanup() override;
    private:
        D3D12Device& _device;
        ComPtr<ID3D12CommandAllocator> _commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> _commandList;
        
        ComPtr<ID3D12Fence> _fence;
        UINT64 _fenceValue = 0;
        HANDLE _fenceEvent = nullptr;

        std::vector<D3D12RenderTarget> _renderTargets;

        bool CreateCommandAllocator();
        bool CreateCommandList();
        bool CreateFence();
        bool CreateRenderTarget(const sys::WinPtr& window, D3D12RenderTarget* outRenderTarget);

        void DestroyRenderTargets();
        void DestroyFence();
        void DestroyCommandList();
        void DestroyCommandAllocator();
        
        void WaitForFence();
        D3D12_CPU_DESCRIPTOR_HANDLE OffsetHandle(const D3D12RenderTarget& target);
    };
}