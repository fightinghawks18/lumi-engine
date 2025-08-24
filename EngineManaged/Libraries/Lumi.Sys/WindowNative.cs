using System;
using System.Runtime.InteropServices;

namespace Lumi.Sys;

internal static partial class WindowNative
{
    [LibraryImport("sysclib", EntryPoint = "window_create")]
    internal static partial IntPtr Create();
    
    [LibraryImport("sysclib", EntryPoint = "window_destroy")]
    internal static partial void Destroy(IntPtr w);
}