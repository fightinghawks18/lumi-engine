find_package(SDL3 CONFIG REQUIRED)

add_library(syslib STATIC
        window.cpp
        window_manager.cpp
)

target_link_libraries(syslib PUBLIC
        SDL3::SDL3
)

target_include_directories(syslib PUBLIC
        ${NATIVE_INCLUDE_DIR}
)

include(${CMACROS}/targets.cmake)
install_target(syslib)