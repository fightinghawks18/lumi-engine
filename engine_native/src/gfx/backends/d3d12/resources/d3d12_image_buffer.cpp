#include <resources/d3d12_image_buffer.h>
#include <d3d12_device.h>
#include <debugging/logger.h>

#include <utils/d3d12_image_utils.h>

namespace lumi::gfx::d3d12::resources
{
    D3D12ImageBuffer::D3D12ImageBuffer(D3D12Device& device)
        : _device(device)
    {}

    D3D12ImageBuffer::D3D12ImageBuffer(D3D12Device& device, ComPtr<ID3D12Resource> buffer)
        : _device(device), _res(buffer)
    {
        D3D12_RESOURCE_DESC desc = buffer->GetDesc();
        ImageDesc imgDesc = {};
        imgDesc.format = utils::FromD3D12Format(desc.Format);
        imgDesc.width = static_cast<uint32_t>(desc.Width);
        imgDesc.height = static_cast<uint32_t>(desc.Height);
        imgDesc.usage = ImageUsage::Undefined;
        SetDesc(imgDesc);
    }

    D3D12ImageBuffer::~D3D12ImageBuffer()
    {
        Destroy();
    }

    bool D3D12ImageBuffer::Create()
    {
        if (_res)
        {
            debugging::Logger::Instance().LogWarn("Please destroy the current image before creating one");
            return false;
        }

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = _description.width;
        desc.Height = _description.height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = utils::ChooseD3D12Format(_description.format);
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = utils::ChooseD3D12Flags(_description.usage);

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_CLEAR_VALUE clearValue = {};
        const D3D12_CLEAR_VALUE* pClearValue = nullptr;

        if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        {
            clearValue.Format = desc.Format;
            clearValue.Color[0] = 0.0f;
            clearValue.Color[1] = 0.0f;
            clearValue.Color[2] = 0.0f;
            clearValue.Color[3] = 1.0f;
            pClearValue = &clearValue;
        }
        else if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) == D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        {
            clearValue.Format = desc.Format;
            clearValue.DepthStencil.Depth = 1.0f;
            clearValue.DepthStencil.Stencil = 0;
            pClearValue = &clearValue;
        }

        HRESULT hr = _device.Get()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            pClearValue,
            IID_PPV_ARGS(&_res)
        );

        if (debugging::Logger::Instance().LogIfHRESULTFailure(hr, "Failed to create D3D12ImageBuffer"))
        {
            debugging::Logger::Instance().LogInfo(
                "Image description:\n \tWidth: {}\n \tHeight: {}\n \tFormat: {}\n \tFlags: {}",
                desc.Width,
                desc.Height,
                static_cast<int>(desc.Format),
                static_cast<unsigned int>(desc.Flags)
            );
            return false;
        }
        
        _rtvIndex = _device.AllocateRTVIndex();
        if (_rtvIndex < 0)
        {
            debugging::Logger::Instance().LogError(
                "Cannot allocate a new rtv index, out of rtv descriptors"
            );
            return false;
        }

        if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
        {

            _rtvIndex = _device.AllocateRTVIndex();
            if (_rtvIndex < 0)
            {
                debugging::Logger::Instance().LogError(
                    "Cannot allocate a new rtv index, out of rtv descriptors"
                );
                return false;
            }

            auto rtvHeap = _device.GetRTVHeap();
            if (!rtvHeap)
            {
                debugging::Logger::Instance().LogError("Image Buffers require the device to have a valid rtv heap!");
                return false;
            }

            UINT descriptorSize = _device.GetRTVDescriptorSize();
            _handle.ptr = rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + _rtvIndex * descriptorSize;
            _device.Get()->CreateRenderTargetView(_res.Get(), nullptr, _handle);
        }
        return true;
    }

    void D3D12ImageBuffer::Transition(const ImageState& toState)
    {
        if (_state == toState) return;

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = _res.Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = utils::ChooseD3D12State(_state);
        barrier.Transition.StateAfter = utils::ChooseD3D12State(toState);
        _commandList->ResourceBarrier(1, &barrier);
        
        _state = toState;
    }

    void D3D12ImageBuffer::Destroy()
    {
        if (_rtvIndex >= 0) 
        {
            _device.FreeRTVIndex(_rtvIndex);
            _rtvIndex = -1;
        }
        _res.Reset();
    }
}