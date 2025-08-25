#pragma once

#include <stdint.h>
#include <utils/c_macros.h>
#include "export.h"

C_API_BEGIN

C_API_OPAQUE_STRUCT(WindowManager);
C_API_OPAQUE_STRUCT(Window);
C_API_STRUCT WindowProperties {
    char title[256];
    char icon[256];
    int x, y, w, h;
    int wMin, wMax;
    int hMin, hMax;
    int mode;
    uint8_t resizable;
    uint8_t bordered;
} WindowProperties;

C_API_FUNC(SYS_C_API, WindowManager*, window_manager_create, void);
C_API_FUNC(SYS_C_API, Window*, window_manager_create_window, WindowManager* wm, WindowProperties props);
C_API_FUNC(SYS_C_API, void, window_manager_update, WindowManager* wm);
C_API_FUNC(SYS_C_API, void, window_manager_destroy, WindowManager* wm);

C_API_END