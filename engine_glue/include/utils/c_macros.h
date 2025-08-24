#pragma once

#ifdef __cplusplus
    #define C_API_BEGIN extern "C" {
    #define C_API_END }
#else
    #define C_API_BEGIN
    #define C_API_END
#endif
#define C_API_STRUCT typedef struct

#define C_API_OPAQUE_STRUCT(name) \
        C_API_STRUCT name name

#define C_API_FUNC(api_name, return_type, name, ...) \
        api_name return_type name(__VA_ARGS__)