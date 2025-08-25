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

# Add subdirectories to collect libraries
add_subdirectory(engine_native)
add_subdirectory(engine_glue)
 
# Add C++ tests
add_subdirectory(tests)

# Install C# after building
if(WIN32)
    set(DOTNET_RUNTIME win-x64)
elseif(APPLE)
    set(DOTNET_RUNTIME osx-x64)
elseif(UNIX)
    set(DOTNET_RUNTIME linux-x64)
else()
    set(DOTNET_RUNTIME win-x64) # Default fallback
endif()

add_custom_target(dotnet_publish ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX}/bin"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/bin"
    COMMAND dotnet restore "${PROJECT_DIR}/EngineManaged/EngineManaged.sln"
    COMMAND dotnet publish -c ${CMAKE_BUILD_TYPE} "${PROJECT_DIR}/EngineManaged/EngineManaged.sln"
    COMMENT "Running install and then publishing .NET project"
)