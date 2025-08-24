#pragma once

#include <utils/c_macros.h>
#include "export.h"

C_API_BEGIN

C_API_OPAQUE_STRUCT(Window);

C_API_FUNC(SYS_C_API, Window*, window_create, void);
C_API_FUNC(SYS_C_API, void, window_destroy, Window* w);

C_API_END