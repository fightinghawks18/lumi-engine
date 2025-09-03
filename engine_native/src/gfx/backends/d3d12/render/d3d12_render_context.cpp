#include <render/d3d12_render_context.h>
#include <d3d12_render_target.h>

#include <debugging/logger.h>

namespace lumi::gfx::d3d12::render
{
    void D3D12RenderContext::SetRenderTarget(IRenderTarget& window)
    {
        auto* renderTarget = dynamic_cast<D3D12RenderTarget*>(&window);
        if (!renderTarget)
        {
            return;
        }

        _commandAllocator = renderTarget->GetCommandAllocator(_frameNum);
        _commandList = renderTarget->GetCommandList(_frameNum);
    }

    void D3D12RenderContext::BeginRecording(const RenderInfo& info)
    {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
        for (const auto& colorInfo : info.color)
        {
            auto* d3d12Image = dynamic_cast<D3D12ImageBuffer*>(colorInfo.image);
            if (d3d12Image)
            {
                rtvHandles.push_back(d3d12Image->GetRTVHandle());
            }
        }

        D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr;
        if (info.depth)
        {
            auto* d3d12Image = dynamic_cast<D3D12ImageBuffer*>(info.depth->image);
            if (d3d12Image)
            {
                dsvHandle = &(d3d12Image->GetDSVHandle());
            }
        }

        D3D12_VIEWPORT d3d12Viewport;
        D3D12_RECT d3d12Rect;
        
        Viewport infoViewport = info.view.view;
        Scissor infoScissor = info.view.scissor;

        d3d12Viewport.Height = infoViewport.height;
        d3d12Viewport.Width = infoViewport.width;
        d3d12Viewport.MaxDepth = infoViewport.maxDepth;
        d3d12Viewport.MinDepth = infoViewport.minDepth;
        d3d12Viewport.TopLeftX = infoViewport.x;
        d3d12Viewport.TopLeftY = infoViewport.y;

        d3d12Rect.left = infoScissor.x;
        d3d12Rect.top = infoScissor.y;
        d3d12Rect.right = infoScissor.x + infoScissor.width;
        d3d12Rect.bottom = infoScissor.y + infoScissor.height;
        
        _commandList->RSSetViewports(1, &d3d12Viewport);
        _commandList->RSSetScissorRects(1, &d3d12Rect);

        if (!rtvHandles.empty())
            _commandList->OMSetRenderTargets(static_cast<UINT>(rtvHandles.size()), rtvHandles.data(), FALSE, nullptr);

        for (size_t i = 0; i < info.color.size(); ++i)
        {
            const auto& colorInfo = info.color[i];
            auto colorImage = rtvHandles[i];
            const auto& colorArray = colorInfo.color;
            switch (colorInfo.loadOp)
            {
                case RenderLoadOp::Clear:
                {
                    FLOAT clearColor[4] = { colorArray[0], colorArray[1], colorArray[2], colorArray[3] };
                    _commandList->ClearRenderTargetView(colorImage, clearColor, 0, nullptr);
                }
                case RenderLoadOp::Load:
                    break;
                case RenderLoadOp::DontCare:
                    break;
            }
        }

        if (dsvHandle)
        {
            const auto& depthInfo = info.depth;
            const auto& depthArray = depthInfo->depthStencil;

            switch (depthInfo->loadOp)
            {
                case RenderLoadOp::Clear:
                {
                    FLOAT depthValue = depthArray[0];
                    UINT8 stencilValue = static_cast<UINT8>(depthArray[1]);
                    _commandList->ClearDepthStencilView(
                        *dsvHandle,
                        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                        depthValue,
                        stencilValue,
                        0,
                        nullptr
                    );
                    break;
                }
                case RenderLoadOp::DontCare:
                    break;
                case RenderLoadOp::Load:
                    break;
            }
        }
    }

    void D3D12RenderContext::EndRecording(const RenderInfo& info)
    {
        for (size_t i = 0; i < info.color.size(); ++i)
        {
            auto& colorInfo = info.color[i];
            auto* img = dynamic_cast<D3D12ImageBuffer*>(colorInfo.image);
            if (!img) 
            {
                debugging::Logger::Instance().LogError(
                    "D3D12 cannot handle store operation on color image {}, the image doesn't cast back to D3D12ImageBuffer!",
                    i
                );
                continue;
            }
            switch (colorInfo.storeOp)
            {
                case RenderStoreOp::DontCare:
                {
                    auto resource = static_cast<ID3D12Resource*>(img->Get());
                    if (!resource) 
                    {
                        debugging::Logger::Instance().LogError(
                            "D3D12 attempted to discard an image resource on color image {} via RenderStoreOp::DontCare, but it has no resource!",
                            i
                        );
                        break;
                    }
                    _commandList->DiscardResource(resource, nullptr);
                    break;
                }
                case RenderStoreOp::Store:
                    break;
            }
        }

        if (info.depth)
        {
            auto& depthInfo = info.depth;
            auto* img = dynamic_cast<D3D12ImageBuffer*>(depthInfo->image);
            if (!img)
            {
                debugging::Logger::Instance().LogError(
                    "D3D12 cannot handle store operation on depth image, the image doesn't cast back to D3D12ImageBuffer!"
                );
                _commandList->Close();
                return;
            }
            switch (depthInfo->storeOp)
            {
                case RenderStoreOp::DontCare:
                {
                    auto resource = static_cast<ID3D12Resource*>(img->Get());
                    if (!resource) 
                    {
                        debugging::Logger::Instance().LogError(
                            "D3D12 attempted to discard an image resource on depth image via RenderStoreOp::DontCare, but it has no resource!"
                        );
                        break;
                    }
                    _commandList->DiscardResource(resource, nullptr);
                    break;
                }
                case RenderStoreOp::Store:
                    break;
            }
        }

        _commandList->Close();
    }
}