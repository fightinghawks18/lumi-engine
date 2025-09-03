#pragma once

#include <queue>

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

        int AllocateRTVIndex() 
        {
            if (!_freeRTVIndices.empty()) 
            {
                int idx = _freeRTVIndices.front();
                _freeRTVIndices.pop();
                return idx;
            }
            if (_nextRTVIndex < _rtvHeapSize)
                return _nextRTVIndex++;
            return -1;
        }

        void FreeRTVIndex(int idx) {
            _freeRTVIndices.push(idx);
        }
        int GetRTVHeapSize() const { return _rtvHeapSize; }

        bool IsDeviceLost() const {
            if (!_device) return true;
            HRESULT reason = _device->GetDeviceRemovedReason();
            return reason != S_OK;
        }

        unsigned int GetDeviceRemovedReason() const {
            if (!_device) return 0xFFFFFFFF;
            return static_cast<unsigned int>(_device->GetDeviceRemovedReason());
        }
        
        [[nodiscard]] ComPtr<ID3D12Device> Get() { return _device; }
        [[nodiscard]] ComPtr<IDXGIFactory6> GetFactory() { return _dxgiFactory; }
        [[nodiscard]] ComPtr<ID3D12CommandQueue> GetCommandQueue() { return _commandQueue; }
        [[nodiscard]] ComPtr<ID3D12DescriptorHeap> GetRTVHeap() { return _rtvHeap; }
        [[nodiscard]] UINT GetRTVDescriptorSize() const { return _rtvDescriptorSize; }
    private:
        ComPtr<ID3D12Device> _device;

        #if defined(_DEBUG)
        ComPtr<ID3D12Debug> _debugger;
        #endif

        ComPtr<IDXGIFactory6> _dxgiFactory;
        ComPtr<IDXGIAdapter1> _adapter;
        ComPtr<ID3D12CommandQueue> _commandQueue;
        ComPtr<ID3D12DescriptorHeap> _rtvHeap;
        UINT _rtvDescriptorSize = 0;

        std::queue<int> _freeRTVIndices;
        int _nextRTVIndex = 0;
        int _rtvHeapSize = 64;

        bool CreateDXGIFactory();
        bool ChooseAdapter();
        bool CreateD3D12Device();
        bool CreateCommandQueue();
        bool CreateRTVHeap();

        void DestroyRTVHeap();
        void DestroyCommandQueue();
        void DestroyD3D12Device();
        void DestroyAdapter();
        void DestroyDXGIFactory();
    };
}