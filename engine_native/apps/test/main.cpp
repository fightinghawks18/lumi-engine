#undef CreateWindow
#undef CreateWindowExW
#undef CreateWindowEx

#include <debugging/logger.h>
#include <sys/window_manager.h>
#include <gfx/backends/d3d12/d3d12_device.h>
#include <gfx/backends/d3d12/d3d12_render_target.h>
#include <gfx/backends/d3d12/render/d3d12_render_context.h>

using namespace lumi;
using namespace gfx;

int main()
{
    sys::WindowManager windowManager;
    if (!windowManager.Init())
    {
        return -1;
    }
    
    sys::WindowProperties winProps = {};
    winProps.title = "Test Window";
    winProps.icon = "";
    winProps.x = SDL_WINDOWPOS_CENTERED;
    winProps.y = SDL_WINDOWPOS_CENTERED;
    winProps.w = 800;
    winProps.h = 600;
    winProps.hMin = 600;
    winProps.hMax = 1200;
    winProps.wMin = 800;
    winProps.wMax = 1600;
    winProps.mode = sys::WindowMode::Windowed;
    winProps.bordered = true;
    winProps.resizable = true;

    sys::WinPtr window = windowManager.NewWindow(winProps);
    if (!window)
    { 
        debugging::Logger::Instance().LogError("Window failure!");
        return -1;
    }
    
    d3d12::D3D12Device device;
    if (!device.Init())
    {
        debugging::Logger::Instance().LogError("Device failure!");
        return -1;
    }

    uint32_t maxFramesInFlight = 3;
    
    std::shared_ptr<d3d12::D3D12RenderTarget> renderTarget = std::make_shared<d3d12::D3D12RenderTarget>(device, window);
    if (!renderTarget->Init(maxFramesInFlight))
    {
        debugging::Logger::Instance().LogError("Render Target failure!");
        return -1;
    }

    render::RenderOrchestrator renderOrchestrator;
    d3d12::render::D3D12RenderContext renderContext;

    render::RenderPass pass;
    pass.targets = { renderTarget.get() };
    pass.execute = [&](render::IRenderContext& ctx, IRenderTarget* target)
    {
        render::Viewport viewport;
        viewport.height = static_cast<float>(target->GetHeight());
        viewport.width = static_cast<float>(target->GetWidth());
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        viewport.x = 0.0f;
        viewport.y = 0.0f;

        render::Scissor scissor;
        scissor.height = static_cast<int>(viewport.height);
        scissor.width = static_cast<int>(viewport.height);
        scissor.x = static_cast<int>(viewport.x);
        scissor.y = static_cast<int>(viewport.y);

        render::RenderView view = { viewport, scissor };

        render::RenderColorInfo color;
        color.color[0] = 0.2f;
        color.color[1] = 0.2f;
        color.color[2] = 0.2f;
        color.color[3] = 1.0f;

        color.image = target->GetColorBuffer(ctx.GetFrameNumber()).get();

        render::RenderInfo info;
        info.view = view;
        info.color = { color };
        info.depth = nullptr;
        
        ctx.BeginRecording(info);
        ctx.EndRecording(info);
    };

    renderOrchestrator.NewPass("main", pass);
    
    uint32_t frameIndex = 0;
    while (true)
    {
        if (window->Closing())
        {
            break;
        }
        
        windowManager.Update();

        renderTarget->StartRendering(frameIndex);

        renderContext.SetFrameNumber(frameIndex);
        renderContext.SetRenderTarget(*renderTarget.get());
        renderOrchestrator.Execute(renderContext);

        renderTarget->EndRendering(frameIndex);

        frameIndex = (frameIndex + 1) % maxFramesInFlight;
    }
    
    renderTarget.reset();
    device.Cleanup();
    windowManager.Cleanup();
    return 0;
}