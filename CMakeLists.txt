# This is for engine_glue and engine_native as they are C++ folders
# engine_managed hosts .NET projects (C# folders)
cmake_minimum_required(VERSION 3.10)
project(Engine)

# Configure
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/../lumi" CACHE PATH "Install path" FORCE) # All files install to lumi directory
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Allow exporting all symbols without declspec 
endif()

# Variables
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMACROS "${PROJECT_DIR}/cmake") # CMake macros
set(BINARY_EXTS dll dylib so exe)
set(LIBRARY_EXTS lib a)

# Add subdirectories to collect libraries
add_subdirectory(engine_native)
add_subdirectory(engine_glue)

# Add C++ applications (for testing)
add_subdirectory(apps/native)
 
# Add C++ tests
add_subdirectory(tests)