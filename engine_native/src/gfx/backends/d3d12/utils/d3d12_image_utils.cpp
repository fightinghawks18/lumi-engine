#include <utils/d3d12_image_utils.h>
#include <dxgiformat.h>

namespace lumi::gfx::d3d12::utils
{
    DXGI_FORMAT ChooseD3D12Format(const ImageFormat& format)
    {
        // TODO: Support more formats
        switch (format)
        {
            case ImageFormat::RGBA8: 
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case ImageFormat::RGBA16F:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case ImageFormat::RGBA32F:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case ImageFormat::Depth24Stencil8:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    ImageFormat FromD3D12Format(DXGI_FORMAT format)
    {
        // TODO: Support more formats
        switch (format)
        {
            case DXGI_FORMAT_R8G8B8A8_UNORM: 
                return ImageFormat::RGBA8;
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return ImageFormat::RGBA16F;
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return ImageFormat::RGBA32F;
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
                return ImageFormat::Depth24Stencil8;
            default:
                return ImageFormat::Undefined;
        }
    }

    D3D12_RESOURCE_FLAGS ChooseD3D12Flags(const ImageUsage& usage)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
        if ((usage & ImageUsage::Render) == ImageUsage::Render)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if ((usage & ImageUsage::DepthStencil) == ImageUsage::DepthStencil)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if ((usage & ImageUsage::UAV) == ImageUsage::UAV)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        return flags;
    }

    D3D12_RESOURCE_STATES ChooseD3D12State(const ImageState& state)
    {
        switch (state)
        {
            case ImageState::Undefined:
                return D3D12_RESOURCE_STATE_COMMON;
            case ImageState::Color:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case ImageState::DepthStencil:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case ImageState::Shader:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            case ImageState::UAV:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case ImageState::Present:
                return D3D12_RESOURCE_STATE_PRESENT;
            default:
                return D3D12_RESOURCE_STATE_COMMON;
        }
    }
}