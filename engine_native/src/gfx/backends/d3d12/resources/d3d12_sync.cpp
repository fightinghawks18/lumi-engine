#include <resources/d3d12_sync.h>
#include <debugging/logger.h>

namespace lumi::gfx::d3d12::resources
{
    D3D12Sync::~D3D12Sync() { Destroy(); }

    bool D3D12Sync::Init(D3D12Device& device, int frameCount)
    {
        _frameCount = frameCount;
        _fenceValues.resize(frameCount, 0);

        if (debugging::Logger::Instance().LogIfHRESULTFailure(
            device.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)),
            "Failed to create a D3D12 fence"
        ))
        {
            return false;
        }

        _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!_fenceEvent)
        {
            debugging::Logger::Instance().LogError("Failed to create D3D12 fence event");
            return false;
        }
        return true;
    }

    void D3D12Sync::Signal(ID3D12CommandQueue* queue, int frameIndex)
    {
        _fenceValues[frameIndex]++;
        queue->Signal(_fence.Get(), _fenceValues[frameIndex]);
    }

    void D3D12Sync::WaitForFrame(int frameIndex)
    {
        if (_fence->GetCompletedValue() < _fenceValues[frameIndex])
        {
            _fence->SetEventOnCompletion(_fenceValues[frameIndex], _fenceEvent);
            WaitForSingleObject(_fenceEvent, INFINITE);
        }
    }

    void D3D12Sync::AdvanceFrame()
    {
        _currentFrame = (_currentFrame + 1) % _frameCount;
    }

    void D3D12Sync::Destroy()
    {
        if (_fenceEvent) {
            CloseHandle(_fenceEvent);
            _fenceEvent = nullptr;
        }
        _fence.Reset();
        _fenceValues.clear();
    }

    UINT64 D3D12Sync::GetFenceValue(int frameIndex) const
    {
        return _fenceValues[frameIndex];
    }
}