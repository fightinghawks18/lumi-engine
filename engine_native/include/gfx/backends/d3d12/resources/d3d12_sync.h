#pragma once

#include <gfx/backends/d3d12/d3d12_device.h>

namespace lumi::gfx::d3d12::resources
{
    class D3D12Sync
    {
    public:
        ~D3D12Sync();

        bool Init(D3D12Device& device, int frameCount);
        void Signal(ID3D12CommandQueue* queue, int frameIndex);
        void WaitForFrame(int frameIndex);
        void AdvanceFrame();
        void Destroy();

        UINT64 GetFenceValue(int frameIndex) const;
    private:
        ComPtr<ID3D12Fence> _fence;
        std::vector<UINT64> _fenceValues;
        HANDLE _fenceEvent = nullptr;
        int _frameCount = 0;
        int _currentFrame = 0;
    };
}