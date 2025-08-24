namespace Lumi.Sys;
public class Window
{
    private IntPtr _nativePtr;

    internal Window(IntPtr nativeHandle) => _nativePtr = nativeHandle;
    public Window() => _nativePtr = WindowNative.Create();

    public void Delete()
    {
        if (_nativePtr != IntPtr.Zero)
        {
            WindowNative.Destroy(_nativePtr);
            _nativePtr = IntPtr.Zero;
        }
    }
}
