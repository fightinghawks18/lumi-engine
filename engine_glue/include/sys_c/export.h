#pragma once

#ifdef _WIN32
  #ifdef SYS_C_EXPORTS
    #define SYS_C_API __declspec(dllexport)
  #else
    #define SYS_C_API __declspec(dllimport)
  #endif
#else
  #define SYS_C_API
#endif