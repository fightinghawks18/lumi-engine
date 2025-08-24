using System;
using System.Runtime.InteropServices;

namespace Lumi.Sys;

internal static partial class WindowNative
{
    [LibraryImport("sysclib", EntryPoint = "window_create")]
    internal static partial IntPtr Create();

    [LibraryImport("sysclib", EntryPoint = "window_closing")]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool Closing(IntPtr w);
    
    [LibraryImport("sysclib", EntryPoint = "window_destroy")]
    internal static partial void Destroy(IntPtr w);
}