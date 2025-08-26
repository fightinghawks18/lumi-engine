#include <gfx/backends/d3d12/d3d12_renderer.h>
#include <debugging/logger.h>

#include <comdef.h>

namespace lumi::gfx::d3d12
{
    D3D12Renderer::D3D12Renderer(D3D12Device& device)
        : _device(device)
    {}

    bool D3D12Renderer::Init()
    {
        if (!CreateCommandAllocator()) return false;
        if (!CreateCommandList()) return false;
        if (!CreateFence()) return false;
        return true;
    }

    D3D12RenderTarget D3D12Renderer::AddRenderTarget(const sys::WinPtr& window)
    {
        D3D12RenderTarget renderTarget{};
        if (!CreateRenderTarget(window, &renderTarget))
        {
            debugging::Logger::Instance().LogError(
                "Failed to create render target for window {}",
                window->GetID()
            );
        }
        return renderTarget;
    }

    void D3D12Renderer::RemoveRenderTarget(const sys::WinPtr& window)
    {
        auto it = std::remove_if(_renderTargets.begin(), _renderTargets.end(),
        [&](D3D12RenderTarget& rt) 
        {
            if (rt.window == window)
            {
                rt.swapChain.Reset();
                rt.rtvHeap.Reset();
                for (auto& res : rt.renderTargets)
                    res.Reset();
                rt.renderTargets.clear();
                return true;
            }
            return false;
        });
        _renderTargets.erase(it, _renderTargets.end());
    }

    void D3D12Renderer::ResizeRenderTarget(D3D12RenderTarget& target, UINT width, UINT height)
    {
        WaitForFence();

        for (auto& rt : target.renderTargets)
            rt.Reset();
        target.renderTargets.clear();

        DXGI_SWAP_CHAIN_DESC desc = {};
        target.swapChain->GetDesc(&desc);
        HRESULT hr = target.swapChain->ResizeBuffers(
            desc.BufferCount, width, height, desc.BufferDesc.Format, desc.Flags
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to resize swapchain buffers (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = OffsetHandle(target);
        target.renderTargets.resize(desc.BufferCount);
        for (UINT i = 0; i < desc.BufferCount; ++i)
        {
            hr = target.swapChain->GetBuffer(i, IID_PPV_ARGS(&target.renderTargets[i]));
            if (FAILED(hr))
            {
                _com_error err(hr);
                debugging::Logger::Instance().LogError(
                    "Failed to get swapchain buffer (HRESULT: 0x{:08X}): {}", 
                    static_cast<unsigned int>(hr), err.ErrorMessage()
                );
                return;
            }
            _device.Get()->CreateRenderTargetView(target.renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += target.rtvDescriptorSize;
        }

        target.frameIndex = target.swapChain->GetCurrentBackBufferIndex();
    }

    void D3D12Renderer::StartRender(D3D12RenderTarget& target)
    {
        if (target.OutOfDate())
        {
            const auto width = static_cast<UINT>(target.window->GetWidth());
            const auto height = static_cast<UINT>(target.window->GetHeight());
            ResizeRenderTarget(target, width, height);
        }

        _commandAllocator->Reset();
        _commandList->Reset(_commandAllocator.Get(), nullptr);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = OffsetHandle(target);
        _commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = target.renderTargets[target.frameIndex].Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        _commandList->ResourceBarrier(1, &barrier);

        const float clearColor[] = {0.1f, 0.15f, 0.1f, 1.0f};
        _commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    void D3D12Renderer::EndRender(D3D12RenderTarget& target)
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = target.renderTargets[target.frameIndex].Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        _commandList->ResourceBarrier(1, &barrier);

        _commandList->Close();
        ID3D12CommandList* ppCommandLists[] = {_commandList.Get()};
        _device.GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);

        target.swapChain->Present(1, 0);

        WaitForFence();

        target.frameIndex = target.swapChain->GetCurrentBackBufferIndex();
    }

    void D3D12Renderer::Cleanup()
    {
        DestroyRenderTargets();
        DestroyFence();
        DestroyCommandList();
        DestroyCommandAllocator();
    }

    bool D3D12Renderer::CreateCommandAllocator()
    {
        HRESULT hr = _device.Get()->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create command allocator (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        return true;
    }

    bool D3D12Renderer::CreateCommandList()
    {
        HRESULT hr = _device.Get()->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
            _commandAllocator.Get(), nullptr, 
            IID_PPV_ARGS(&_commandList)
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create command list (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        _commandList->Close();
        return true;
    }

    bool D3D12Renderer::CreateFence()
    {
        HRESULT hr = _device.Get()->CreateFence(
            0, D3D12_FENCE_FLAG_NONE, 
            IID_PPV_ARGS(&_fence)
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create fence (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }

        _fenceValue = 1;
        _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!_fenceEvent)
        {
            debugging::Logger::Instance().LogError(
                "Failed to create fence event"
            );
            return false;
        }
        return true;
    }

    bool D3D12Renderer::CreateRenderTarget(const sys::WinPtr& window, D3D12RenderTarget* outRenderTarget)
    {
        if (!window || !outRenderTarget)
            return false;
        
        D3D12RenderTarget renderTarget{};
        renderTarget.window = window;
        
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Width = window->GetWidth();
        swapChainDesc.Height = window->GetHeight();
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window->GetHandle()), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL));

        ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = _device.GetFactory()->CreateSwapChainForHwnd(
            _device.GetCommandQueue().Get(), // Command queue
            hwnd, // HWND
            &swapChainDesc,
            nullptr, nullptr,
            &swapChain1
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create swap chain (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }

        hr = swapChain1.As(&renderTarget.swapChain);
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to query IDXGISwapChain4 (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = _device.Get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&renderTarget.rtvHeap));
        if (FAILED(hr)) 
        { 
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create descriptor heap (HRESULT: 0x{:08X}): {}", 
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        renderTarget.rtvDescriptorSize = _device.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        renderTarget.renderTargets.resize(swapChainDesc.BufferCount);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTarget.rtvHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < swapChainDesc.BufferCount; ++i)
        {
            hr = renderTarget.swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget.renderTargets[i]));
            if (FAILED(hr)) 
            { 
                _com_error err(hr);
                debugging::Logger::Instance().LogError(
                    "Failed to get swapchain buffer (HRESULT: 0x{:08X}): {}", 
                    static_cast<unsigned int>(hr), err.ErrorMessage()
                );
                return false;
            }
            _device.Get()->CreateRenderTargetView(renderTarget.renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += renderTarget.rtvDescriptorSize;
        }
        renderTarget.frameIndex = renderTarget.swapChain->GetCurrentBackBufferIndex();

        _renderTargets.push_back(renderTarget);
        *outRenderTarget = renderTarget;
        return true;
    }

    void D3D12Renderer::DestroyRenderTargets() 
    {
        for (auto& target : _renderTargets)
        {
            RemoveRenderTarget(target.window);
        }
        _renderTargets.clear();
    }

    void D3D12Renderer::DestroyFence() 
    {
        _fence.Reset();
    }

    void D3D12Renderer::DestroyCommandList() 
    {
        _commandList.Reset();
    }

    void D3D12Renderer::DestroyCommandAllocator() 
    {
        _commandAllocator.Reset();
    }

    void D3D12Renderer::WaitForFence()
    {
        const UINT64 fenceToWaitFor = ++_fenceValue;
        _device.GetCommandQueue()->Signal(_fence.Get(), fenceToWaitFor);
        if (_fence->GetCompletedValue() < fenceToWaitFor)
        {
            _fence->SetEventOnCompletion(fenceToWaitFor, _fenceEvent);
            WaitForSingleObject(_fenceEvent, INFINITE);
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12Renderer::OffsetHandle(const D3D12RenderTarget& target)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = target.rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += target.frameIndex * target.rtvDescriptorSize;
        return rtvHandle;
    }
}