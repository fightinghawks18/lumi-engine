namespace Lumi.Sys;
public class Window
{
    private IntPtr _nativePtr;

    internal Window(IntPtr nativeHandle) => _nativePtr = nativeHandle;
    public bool Closing() => WindowNative.Closing(_nativePtr);
    public void Delete()
    {
        if (_nativePtr != IntPtr.Zero)
        {
            WindowNative.Destroy(_nativePtr);
            _nativePtr = IntPtr.Zero;
        }
    }
}
