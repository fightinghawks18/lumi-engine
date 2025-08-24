add_library(sysclib SHARED
        window_manager.cpp
        window.cpp
)

target_link_libraries(sysclib PRIVATE
        syslib
)

target_include_directories(sysclib PUBLIC
        ${GLUE_INCLUDE_DIR}
)

target_compile_definitions(sysclib PUBLIC SYS_C_EXPORTS)

include(${CMACROS}/targets.cmake)
install_target(sysclib)