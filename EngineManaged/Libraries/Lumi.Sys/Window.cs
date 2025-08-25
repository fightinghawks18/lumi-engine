namespace Lumi.Sys;

public class Window
{
    private IntPtr _nativePtr;
    public static int GetWindowPosCentered() => 0x2FFF0000;

    internal Window(IntPtr nativeHandle) => _nativePtr = nativeHandle;
    public void Warp(int x, int y) => WindowNative.Warp(_nativePtr, x, y);
    public void WarpRelative(int x, int y) => WindowNative.WarpRelative(_nativePtr, x, y);
    public void Resize(int w, int h) => WindowNative.Resize(_nativePtr, w, h);
    public void SetName(string name) => WindowNative.SetName(_nativePtr, name);
    public void SetIcon(string icon) => WindowNative.SetIcon(_nativePtr, icon);
    public void SetSizeLimits(int wMin, int wMax, int hMin, int hMax)
        => WindowNative.SetSizeLimits(_nativePtr, wMin, wMax, hMin, hMax);

    public void SetWidthLimits(int wMin, int wMax)
        => WindowNative.SetWidthLimits(_nativePtr, wMin, wMax);

    public void SetHeightLimits(int hMin, int hMax)
        => WindowNative.SetHeightLimits(_nativePtr, hMin, hMax);

    public void SetResizable(bool resizable)
        => WindowNative.SetResizable(_nativePtr, resizable);

    public void SetBordered(bool bordered)
        => WindowNative.SetBordered(_nativePtr, bordered);

    public void SetMode(WindowMode mode) => WindowNative.SetMode(_nativePtr, mode);

    public bool Closing() => WindowNative.Closing(_nativePtr);
    public void Delete()
    {
        if (_nativePtr != IntPtr.Zero)
        {
            WindowNative.Destroy(_nativePtr);
            _nativePtr = IntPtr.Zero;
        }
    }

    public bool IsWindow() => WindowNative.IsWindow(_nativePtr);
}
