using System;
using System.Runtime.InteropServices;

namespace Lumi.Sys;

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct WindowProperties
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string title;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string icon;
    public int x, y, w, h;
    public int wMin, wMax;
    public int hMin, hMax;
    public WindowMode mode;
    [MarshalAs(UnmanagedType.U1)] public bool resizable;
    [MarshalAs(UnmanagedType.U1)] public bool bordered;
}

public enum WindowMode
{
    Windowed,
    Fullscreen,
    FullscreenBorderless
}

internal static partial class WindowNative
{
    [LibraryImport("sysclib", EntryPoint = "window_warp")]
    internal static partial void Warp(IntPtr w, int x, int y);

    [LibraryImport("sysclib", EntryPoint = "window_warp_relative")]
    internal static partial void WarpRelative(IntPtr w, int x, int y);

    [LibraryImport("sysclib", EntryPoint = "window_resize")]
    internal static partial void Resize(IntPtr win, int w, int h);

    [LibraryImport("sysclib", EntryPoint = "window_set_name", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial void SetName(IntPtr w, string name);

    [LibraryImport("sysclib", EntryPoint = "window_set_icon", StringMarshalling = StringMarshalling.Utf8)]
    internal static partial void SetIcon(IntPtr w, string icon);

    [LibraryImport("sysclib", EntryPoint = "window_set_size_limits")]
    internal static partial void SetSizeLimits(IntPtr w, int wMin, int wMax, int hMin, int hMax);

    [LibraryImport("sysclib", EntryPoint = "window_set_width_limits")]
    internal static partial void SetWidthLimits(IntPtr w, int wMin, int wMax);

    [LibraryImport("sysclib", EntryPoint = "window_set_height_limits")]
    internal static partial void SetHeightLimits(IntPtr w, int hMin, int hMax);

    [LibraryImport("sysclib", EntryPoint = "window_set_resizable")]
    internal static partial void SetResizable(IntPtr w, [MarshalAs(UnmanagedType.U1)] bool resizable);

    [LibraryImport("sysclib", EntryPoint = "window_set_bordered")]
    internal static partial void SetBordered(IntPtr w, [MarshalAs(UnmanagedType.U1)] bool bordered);

    [LibraryImport("sysclib", EntryPoint = "window_set_mode")]
    internal static partial void SetMode(IntPtr w, WindowMode mode);

    [LibraryImport("sysclib", EntryPoint = "window_closing")]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool Closing(IntPtr w);

    [LibraryImport("sysclib", EntryPoint = "window_destroy")]
    internal static partial void Destroy(IntPtr w);

    [LibraryImport("sysclib", EntryPoint = "window_is_valid")]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool IsWindow(IntPtr w);
}