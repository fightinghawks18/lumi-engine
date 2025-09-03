add_library(gfxd3d12backend STATIC
        d3d12_device.cpp
        d3d12_render_target.cpp

        render/d3d12_render_context.cpp
        
        resources/d3d12_image_buffer.cpp
        resources/d3d12_sync.cpp

        utils/d3d12_image_utils.cpp
)

target_include_directories(gfxd3d12backend PUBLIC
        ${NATIVE_INCLUDE_DIR}
        PRIVATE
        ${NATIVE_INCLUDE_DIR}/gfx/backends/d3d12
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