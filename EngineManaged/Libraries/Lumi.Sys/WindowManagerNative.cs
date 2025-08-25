using System;
using System.Runtime.InteropServices;

namespace Lumi.Sys;

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