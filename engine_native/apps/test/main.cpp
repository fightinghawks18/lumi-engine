#undef CreateWindow
#undef CreateWindowExW
#undef CreateWindowEx

#include <debugging/logger.h>
#include <sys/window_manager.h>
#include <gfx/backends/d3d12/d3d12_renderer.h>

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
    
    d3d12::D3D12Renderer renderer(device);
    if (!renderer.Init())
    {
        debugging::Logger::Instance().LogError("Renderer failure!");
        return -1;
    }
    
    d3d12::D3D12RenderTarget renderTarget = renderer.AddRenderTarget(window);
    if (!renderTarget.window)
    {
        debugging::Logger::Instance().LogError("Render Target failure!");
        return -1;
    }
    
    while (true)
    {
        if (window->Closing())
        {
            break;
        }
        
        windowManager.Update();

        renderer.StartRender(renderTarget);
        renderer.EndRender(renderTarget);
    }
    
    renderer.Cleanup();
    device.Cleanup();
    windowManager.Cleanup();
    return 0;
}