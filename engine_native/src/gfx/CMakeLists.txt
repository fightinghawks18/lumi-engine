add_library(gfxlib STATIC
    device.cpp
    renderer.cpp

    render/render_orchestrator.cpp
)

target_include_directories(gfxlib PUBLIC
        ${NATIVE_INCLUDE_DIR}
)

include(${CMACROS}/targets.cmake)
install_target(gfxlib)

add_subdirectory(backends)

if(WIN32)
    target_link_libraries(gfxlib PUBLIC gfxd3d12backend)
endif()