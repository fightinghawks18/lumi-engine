macro(install_target target)
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
    )
endmacro()