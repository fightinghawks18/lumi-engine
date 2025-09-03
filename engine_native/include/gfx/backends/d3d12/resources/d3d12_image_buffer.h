#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <gfx/resources/image_buffer.h>

namespace lumi::gfx::d3d12
{
    class D3D12Device;
}

namespace lumi::gfx::d3d12::resources
{
    using Microsoft::WRL::ComPtr;
    using gfx::resources::IImageBuffer;
    using gfx::resources::ImageFormat;
    using gfx::resources::ImageState;
    using gfx::resources::ImageUsage;
    using gfx::resources::ImageDesc;

    class D3D12ImageBuffer : public IImageBuffer
    {
    public:
        D3D12ImageBuffer(D3D12Device& device);
        D3D12ImageBuffer(D3D12Device& device, ComPtr<ID3D12Resource> buffer);
        ~D3D12ImageBuffer() override;

        bool Create() override;

        void SetCommandList(ComPtr<ID3D12GraphicsCommandList> commandList) { _commandList = commandList; }

        void Transition(const ImageState& toState) override;

        void Destroy() override;
        
        [[nodiscard]] void* Get() { return _res.Get(); }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() { return _handle; }
    private:
        D3D12Device& _device;
        ComPtr<ID3D12Resource> _res;
        D3D12_CPU_DESCRIPTOR_HANDLE _handle;
        ComPtr<ID3D12GraphicsCommandList> _commandList;
        
        int _rtvIndex = 0;
    };
}