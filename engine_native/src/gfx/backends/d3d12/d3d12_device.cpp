#include <gfx/backends/d3d12/d3d12_device.h>
#include <debugging/logger.h>
#include <comdef.h>

namespace lumi::gfx::d3d12
{
    bool D3D12Device::Init()
    {
        if (!CreateDXGIFactory()) return false;
        if (!ChooseAdapter()) return false;
        if (!CreateD3D12Device()) return false;
        if (!CreateCommandQueue()) return false;
        return true;
    }

    void D3D12Device::Cleanup()
    {
        DestroyCommandQueue();
        DestroyD3D12Device();
        DestroyAdapter();
        DestroyDXGIFactory();
    }

    bool D3D12Device::CreateDXGIFactory()
    {
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgiFactory));
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create DXGI Factory (HRESULT: 0x{:08X}): {}",
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        return true;
    }

    bool D3D12Device::ChooseAdapter()
    {
        for (UINT adapterIndex = 0;; ++adapterIndex)
        {
            ComPtr<IDXGIAdapter1> adapter;
            if (_dxgiFactory->EnumAdapters1(adapterIndex, &adapter) == DXGI_ERROR_NOT_FOUND)
                break; // Stop enumerating if we reach the end
            
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Software adapters are not physical gpus
            // Skip them since we need an actual gpu
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;
            
            // Check if adapter supports D3D12
            if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                continue; // Skip if it doesn't support D3D12

            debugging::Logger::Instance().LogInfo(
                "Found adapter after {} enumeration{}",
                adapterIndex+1, adapterIndex > 0 ? "s" : ""
            );

            std::wstring ws(desc.Description);
            std::string adapterName(ws.begin(), ws.end());

            debugging::Logger::Instance().LogInfo(
                "Adapter name: {} \n \tAdapter dedicated video mem: {} \n \tAdapter dedicated system mem: {}",
                adapterName, 
                desc.DedicatedVideoMemory / (1024 * 1024), 
                desc.DedicatedSystemMemory / (1024 * 1024)
            );

            _adapter = adapter;
            return true;
        }

        debugging::Logger::Instance().LogError("Failed to find a suitable adapter for D3D12");
        return false;
    }

    bool D3D12Device::CreateD3D12Device()
    {
        HRESULT hr = D3D12CreateDevice(
            _adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&_device)
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create D3D12 Device (HRESULT: 0x{:08X}): {}",
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        return true;
    }

    bool D3D12Device::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        HRESULT hr = _device->CreateCommandQueue(
            &desc, 
            IID_PPV_ARGS(&_commandQueue)
        );
        if (FAILED(hr))
        {
            _com_error err(hr);
            debugging::Logger::Instance().LogError(
                "Failed to create D3D12 Command Queue (HRESULT: 0x{:08X}): {}",
                static_cast<unsigned int>(hr), err.ErrorMessage()
            );
            return false;
        }
        return true;
    }

    void D3D12Device::DestroyCommandQueue()
    {
        _commandQueue.Reset();
    }

    void D3D12Device::DestroyD3D12Device()
    {
        _device.Reset();
    }

    void D3D12Device::DestroyAdapter()
    {
        _adapter.Reset();
    }

    void D3D12Device::DestroyDXGIFactory()
    {
        _dxgiFactory.Reset();
    }
}