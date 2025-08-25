#pragma once

#include <stdint.h>
#include <utils/c_macros.h>
#include "export.h"

C_API_BEGIN

C_API_OPAQUE_STRUCT(Window);

C_API_FUNC(SYS_C_API, void, window_warp, Window* w, int x, int y);
C_API_FUNC(SYS_C_API, void, window_warp_relative, Window* w, int x, int y);
C_API_FUNC(SYS_C_API, void, window_resize, Window* win, int w, int h);
C_API_FUNC(SYS_C_API, void, window_set_name, Window* w, char* name);
C_API_FUNC(SYS_C_API, void, window_set_icon, Window* w, char* icon);
C_API_FUNC(SYS_C_API, void, window_set_size_limits, Window* w, 
    int wMin, int wMax, 
    int hMin, int hMax);
C_API_FUNC(SYS_C_API, void, window_set_width_limits, Window* w, 
    int wMin, int wMax);
C_API_FUNC(SYS_C_API, void, window_set_height_limits, Window* w,
    int hMin, int hMax);
C_API_FUNC(SYS_C_API, void, window_set_resizable, Window* w, 
    uint8_t resizable);
C_API_FUNC(SYS_C_API, void, window_set_bordered, Window* w,
    uint8_t bordered);
C_API_FUNC(SYS_C_API, void, window_set_mode, Window* w,
    int mode);
C_API_FUNC(SYS_C_API, uint8_t, window_closing, Window* w);
C_API_FUNC(SYS_C_API, void, window_destroy, Window* w);
C_API_FUNC(SYS_C_API, uint8_t, window_is_valid, Window* w);

C_API_END