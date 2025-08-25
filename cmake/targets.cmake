macro(install_target target)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(LIB_DEST "../cmake")
    else()
        set(LIB_DEST "bin")
    endif()

    install(TARGETS ${target}
        RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES
                "api-ms-win-.*"
                "ext-ms-.*"
                "AzureAttest.*"
                "HvsiFileTrust.*"
                "PdmUtilities.*"
            POST_EXCLUDE_REGEXES
                ".*system32.*"
            DIRECTORIES
                "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin"
                "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/bin"
                "${CMAKE_INSTALL_PREFIX}/bin"
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${LIB_DEST}
        ARCHIVE DESTINATION ${LIB_DEST}
        INCLUDES DESTINATION include
    )
endmacro()