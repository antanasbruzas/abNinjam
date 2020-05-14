set (VSTPLUGIN_PRJ_NAME "${PROJECT_NAME}_vst3")
set (VSTPLUGIN_BUNDLE_NAME "${PROJECT_NAME}.vst3")

set (VST3SDK_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/external/VST_SDK/VST3_SDK")
# VST plugin specific settings
include (VSTConfig)

configure_file (VstPluginDefs.h.in "${CMAKE_CURRENT_BINARY_DIR}/VstPluginDefs.h")

# Build VST3 SDK
include("cmake/Vst3.cmake")

# Build the plugin
add_library(${VSTPLUGIN_PRJ_NAME} MODULE
    include/plugcontroller.h
    include/plugids.h
    include/plugprocessor.h
    include/version.h
    include/abvst3editor.h
    include/abuimessagecontroller.h
    include/X11RunLoop.h
    include/metronomevolumeparameter.h
    include/hostcontroller.h
    source/plugfactory.cpp
    source/plugcontroller.cpp
    source/plugprocessor.cpp
    source/abvst3editor.cpp
    source/X11RunLoop.cpp
    source/metronomevolumeparameter.cpp
    source/hostcontroller.cpp)

if(WIN32)
    target_sources(${VSTPLUGIN_PRJ_NAME} PRIVATE vst3.def)
endif()

target_include_directories(${VSTPLUGIN_PRJ_NAME}
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    PREFIX "")

add_dependencies(${VSTPLUGIN_PRJ_NAME} abNinjam::common)
plugin_add_common(${VSTPLUGIN_PRJ_NAME})
plugin_add_vst3sdk(${VSTPLUGIN_PRJ_NAME})
plugin_add_vstgui(${VSTPLUGIN_PRJ_NAME})

# Create the bundle (see "VST 3 Locations / Format")
execute_process (
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/Snapshots")
file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/24F26E6CAA204EB08188B6790CABA3AA_snapshot.png"
    DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/Snapshots/")
file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/24F26E6CAA204EB08188B6790CABA3AA_snapshot_2.0x.png"
    DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/Snapshots/")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/plug.uidesc"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/animation_knob.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/animation_knob_2x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/animation_knob_3x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/background.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/background_2x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/background_3x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/light.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/light_2x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/light_3x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/onoff_button.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/onoff_button_2x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/onoff_button_3x.png"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
else()
    file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/plug-base64.uidesc"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(RENAME "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/plug-base64.uidesc" "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/plug.uidesc")
endif()

if(WIN32)
    set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
        SUFFIX ".vst3"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/${VST3_PACKAGE_ARCHITECTURE}-win")
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${config}" config)
        set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
            "LIBRARY_OUTPUT_DIRECTORY_${config}" "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/${VST3_PACKAGE_ARCHITECTURE}-win")
    endforeach()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win/Plugin.ico"
        "${CMAKE_CURRENT_SOURCE_DIR}/win/desktop.ini"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}")
elseif(APPLE)
    set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
        SUFFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/MacOS")
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${config}" config)
        set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
            "LIBRARY_OUTPUT_DIRECTORY_${config}" "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/MacOS")
    endforeach()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/mac/PkgInfo"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents")
    set(SFIZZ_VST3_BUNDLE_EXECUTABLE "${PROJECT_NAME}")
    set(SFIZZ_VST3_BUNDLE_VERSION "${PROJECT_VERSION}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist"
        "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Info.plist" @ONLY)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/mac/Plugin.icns"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
else()
    set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/${VST3_PACKAGE_ARCHITECTURE}-linux")
endif()

file(COPY "gpl-3.0.txt"
    DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}")

# Do not show warnings thrown by sdk
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${VSTPLUGIN_PRJ_NAME} PRIVATE
        "-Wno-extra"
        "-Wno-multichar"
        "-Wno-reorder"
        "-Wno-ignored-qualifiers"
        "-Wno-unknown-pragmas"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable")
endif()

if (UNIX AND NOT APPLE)
    target_compile_options(${VSTPLUGIN_PRJ_NAME} PRIVATE
        "-Wno-class-memaccess")
    target_link_libraries(${VSTPLUGIN_PRJ_NAME} PRIVATE stdc++fs)
endif()


# Installation
install (DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}"
    DESTINATION "${VSTPLUGIN_INSTALL_DIR}"
    COMPONENT "vst")

