#pragma once
#include <d3d12.h>
#include <gfx/resources/image_buffer.h>

namespace lumi::gfx::d3d12::utils
{
    using gfx::resources::ImageFormat;
    using gfx::resources::ImageState;
    using gfx::resources::ImageUsage;
    using gfx::resources::ImageDesc;

    DXGI_FORMAT ChooseD3D12Format(const ImageFormat& format);
    ImageFormat FromD3D12Format(DXGI_FORMAT format);
    D3D12_RESOURCE_FLAGS ChooseD3D12Flags(const ImageUsage& usage);
    D3D12_RESOURCE_STATES ChooseD3D12State(const ImageState& state);
}