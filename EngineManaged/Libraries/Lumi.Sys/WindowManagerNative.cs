using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace Lumi.Sys;

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct WindowProperties
{  
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string title;
    public int x, y, w, h;
}

internal static partial class WindowManagerNative
{
    [LibraryImport("sysclib", EntryPoint = "window_manager_create")]
    internal static partial IntPtr Create();

    [DllImport("sysclib", EntryPoint = "window_manager_create_window")]
    internal static extern IntPtr CreateWindow(IntPtr wm, WindowProperties props);

    [LibraryImport("sysclib", EntryPoint = "window_manager_update")]
    internal static partial void Update(IntPtr wm);

    [LibraryImport("sysclib", EntryPoint = "window_manager_destroy")]
    internal static partial void Destroy(IntPtr wm);
}