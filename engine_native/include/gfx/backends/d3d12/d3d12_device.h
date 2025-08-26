#pragma once

#include <gfx/device.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace lumi::gfx::d3d12
{
    class D3D12Device : public IDevice
    {
    public:
        bool Init() override;
        void Cleanup() override;
        
        [[nodiscard]] ComPtr<ID3D12Device> Get() { return _device; }
        [[nodiscard]] ComPtr<IDXGIFactory6> GetFactory() { return _dxgiFactory; }
        [[nodiscard]] ComPtr<ID3D12CommandQueue> GetCommandQueue() { return _commandQueue; }
    private:
        ComPtr<ID3D12Device> _device;
        ComPtr<IDXGIFactory6> _dxgiFactory;
        ComPtr<IDXGIAdapter1> _adapter;
        ComPtr<ID3D12CommandQueue> _commandQueue;

        bool CreateDXGIFactory();
        bool ChooseAdapter();
        bool CreateD3D12Device();
        bool CreateCommandQueue();

        void DestroyCommandQueue();
        void DestroyD3D12Device();
        void DestroyAdapter();
        void DestroyDXGIFactory();
    };
}