namespace Lumi.Sys;

public class WindowManager
{
    private IntPtr _nativeHandle;

    public WindowManager()
    {
        _nativeHandle = WindowManagerNative.Create();
    }

    public Window CreateWindow()
    {
        return new Window(WindowManagerNative.CreateWindow(_nativeHandle));
    }

    public void Delete()
    {
        if (_nativeHandle != IntPtr.Zero)
        {
            WindowManagerNative.Destroy(_nativeHandle);
            _nativeHandle = IntPtr.Zero;
        }
    }
}