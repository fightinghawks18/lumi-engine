#include <d3d12_render_target.h>
#include <debugging/logger.h>

namespace lumi::gfx::d3d12
{
    D3D12RenderTarget::D3D12RenderTarget(D3D12Device& device, sys::WinPtr& window)
        : _device(device), _window(window)
    {

    }

    D3D12RenderTarget::~D3D12RenderTarget()
    {
       Cleanup(); 
    }

    bool D3D12RenderTarget::Init(const uint32_t maxInFlight)
    {
        _maxFramesInFlight = maxInFlight;

        if (!CreateSync()) return false;
        if (!CreateSwapChain()) return false;
        if (!CreateImages()) return false;
        if (!CreateCommandAllocator()) return false;
        if (!CreateCommandList()) return false;

        return true;
    }

    void D3D12RenderTarget::Resize(const int width, const int height)
    {
        // Wait for the GPU to finish frames before resizing the buffers
        for (uint32_t i = 0; i < _maxFramesInFlight; ++i)
            _sync->WaitForFrame(i);
        
        // Destroy objects that are dependent on the size
        DestroyCommandList();
        DestroyCommandAllocator();
        DestroyImages();

        DXGI_SWAP_CHAIN_DESC desc;
        _swapChain->GetDesc(&desc);
        auto hr = _swapChain->ResizeBuffers(
            _maxFramesInFlight,
            width,
            height,
            desc.BufferDesc.Format,
            desc.Flags
        );

        if (debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to resize swapchain buffers for window {}", _window->GetID())) 
        {
            return;
        }

        CreateImages();
        CreateCommandAllocator();
        CreateCommandList();
    }

    void D3D12RenderTarget::StartRendering(const uint32_t index)
    {
        if (OutOfDate())
        {
            Resize(static_cast<int>(_window->GetWidth()), static_cast<int>(_window->GetHeight()));
        }

        auto& colorBuffer = _colorBuffers[index];
        auto& depthBuffer = _depthBuffers[index];

        // Reset command objects to discard previous executions
        _commandAllocators[index]->Reset();
        _commandLists[index]->Reset(_commandAllocators[index].Get(), nullptr);

        // Move color to its respective state for initial rendering
        colorBuffer->SetCommandList(_commandLists[index]);
        colorBuffer->Transition(ImageState::Color);

        if (depthBuffer->GetState() != ImageState::DepthStencil)
        {
            depthBuffer->SetCommandList(_commandLists[index]);
            depthBuffer->Transition(ImageState::DepthStencil);
        }
    }

    void D3D12RenderTarget::EndRendering(const uint32_t index)
    {
        auto& colorBuffer = _colorBuffers[index];

        // Move ONLY color to present, depth is not presented to the screen
        colorBuffer->SetCommandList(_commandLists[index]);
        colorBuffer->Transition(ImageState::Present);

        _commandLists[index]->Close();
    }

    void D3D12RenderTarget::SubmitRendering(const uint32_t index)
    {
        // Execute command list
        ID3D12CommandList* lists[] = { _commandLists[index].Get() };
        _device.GetCommandQueue()->ExecuteCommandLists(1, lists);

        // Present
        _swapChain->Present(1, 0);

        // Signal and wait using sync object
        _sync->Signal(_device.GetCommandQueue().Get(), index);
        _sync->WaitForFrame(index);
        _sync->AdvanceFrame();
    }

    bool D3D12RenderTarget::OutOfDate() const
    {
        DXGI_SWAP_CHAIN_DESC desc;
        _swapChain->GetDesc(&desc);
        
        bool sizeOutOfDate = static_cast<uint32_t>(desc.BufferDesc.Width) != _window->GetWidth() 
                             || static_cast<uint32_t>(desc.BufferDesc.Height) != _window->GetHeight();
        if (sizeOutOfDate)
        {
            debugging::Logger::Instance().LogInfo("Window {} size out of date", _window->GetID());
            return true;
        }

        // TODO: Check all images and ensure the format is the same as the render target

        return false;
    }

    void D3D12RenderTarget::Cleanup()
    {
        DestroyCommandList();
        DestroyCommandAllocator();
        DestroyImages();
        DestroySwapChain();
        DestroySync();
    }

    bool D3D12RenderTarget::CreateSync()
    {
        _sync = std::make_shared<D3D12Sync>();
        return _sync->Init(_device, _maxFramesInFlight);
    }

    bool D3D12RenderTarget::CreateSwapChain()
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = _maxFramesInFlight;
        desc.Width = static_cast<UINT>(_window->GetWidth());
        desc.Height = static_cast<UINT>(_window->GetHeight());
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window->GetHandle()), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL));

        if (!hwnd)
        {
            debugging::Logger::Instance().LogError("Failed to find a HWND from window {}", _window->GetID());
            return false;
        }

        auto hr = _device.GetFactory()->CreateSwapChainForHwnd(
            _device.GetCommandQueue().Get(),
            hwnd,
            &desc,
            nullptr, nullptr,
            &_swapChain
        );

        return !debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to create a D3D12 swapchain");
    }

    bool D3D12RenderTarget::CreateImages()
    {
        _colorBuffers.resize(_maxFramesInFlight);
        _depthBuffers.resize(_maxFramesInFlight);

        for (uint32_t i = 0; i < _maxFramesInFlight; ++i)
        {
            // Get swapchain buffer
            ComPtr<ID3D12Resource> buffer;
            auto hr = _swapChain->GetBuffer(i, IID_PPV_ARGS(&buffer));
            if (debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to get swapchain buffer {} for window {}", i, _window->GetID()))
                return false;

            _colorBuffers[i] = std::make_unique<D3D12ImageBuffer>(_device, buffer);

            // Create depth buffer
            resources::ImageDesc depthDesc = {};
            depthDesc.format = resources::ImageFormat::Depth24Stencil8;
            depthDesc.width = _window->GetWidth();
            depthDesc.height = _window->GetHeight();
            depthDesc.usage = resources::ImageUsage::DepthStencil;
            
            _depthBuffers[i] = std::make_unique<D3D12ImageBuffer>(_device);
            _depthBuffers[i]->SetDesc(depthDesc);
            if (!_depthBuffers[i]->Create())
            {
                debugging::Logger::Instance().LogError("Failed to create depth image for index {}", i);
                return false;
            }
        }
        return true;
    }

    bool D3D12RenderTarget::CreateCommandAllocator()
    {
        _commandAllocators.resize(_maxFramesInFlight);
        for (uint32_t i = 0; i < _maxFramesInFlight; ++i)
        {
            auto hr = _device.Get()->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&_commandAllocators[i]));
            if (debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to create D3D12 command allocator for index {}", i))
                return false;
        }
        return true;
    }

    bool D3D12RenderTarget::CreateCommandList()
    {
        _commandLists.resize(_maxFramesInFlight);
        for (uint32_t i = 0; i < _maxFramesInFlight; ++i)
        {
            auto hr = _device.Get()->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                _commandAllocators[i].Get(),
                nullptr,
                IID_PPV_ARGS(&_commandLists[i]));
            if (debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to create D3D12 command list for index {}", i))
                return false;
            _commandLists[i]->Close(); // Start closed
        }
        return true;
    }

    void D3D12RenderTarget::DestroyCommandList()
    {
        for (auto& list : _commandLists)
        {
            list.Reset();
        }
        _commandLists.clear();
    }
    
    void D3D12RenderTarget::DestroyCommandAllocator()
    {
        for (auto& allocator : _commandAllocators)
        {
            allocator.Reset();
        }
        _commandAllocators.clear();
    }
    
    void D3D12RenderTarget::DestroyImages()
    {
        for (auto& images : _colorBuffers)
        {
            images.reset();
        }
        _colorBuffers.clear();

        for (auto& images : _depthBuffers)
        {
            images.reset();
        }
        _depthBuffers.clear();
    }
    
    void D3D12RenderTarget::DestroySwapChain()
    {
        _swapChain.Reset();
    }
    
    void D3D12RenderTarget::DestroySync()
    {
        _sync.reset();
        _sync = nullptr;
    }
}