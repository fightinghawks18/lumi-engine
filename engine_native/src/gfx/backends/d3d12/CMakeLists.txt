add_library(gfxd3d12backend STATIC
        d3d12_device.cpp
        d3d12_renderer.cpp
)

target_include_directories(gfxd3d12backend PUBLIC
        ${NATIVE_INCLUDE_DIR}
)

target_link_libraries(gfxd3d12backend
        PUBLIC
            d3d12
            dxgi
            d3dcompiler
        PRIVATE
            syslib
)

include(${CMACROS}/targets.cmake)
install_target(gfxd3d12backend)