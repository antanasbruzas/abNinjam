if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MakeUninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MakeUninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/MakeUninstall.cmake)

    if (ABNINJAM_VST AND VSTPLUGIN_INSTALL_DIR)
        add_custom_command(TARGET uninstall
            COMMAND rm -rv "${VSTPLUGIN_INSTALL_DIR}/${PROJECT_NAME}.vst3")
    endif()
endif()
