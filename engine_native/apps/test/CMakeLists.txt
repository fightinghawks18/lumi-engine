add_executable(test main.cpp)

target_link_libraries(test PRIVATE
        gfxlib
        syslib    
)

include(${CMACROS}/targets.cmake)
install_target(test)