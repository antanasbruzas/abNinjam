set (VSTPLUGIN_PRJ_NAME "${PROJECT_NAME}_vst3")
set (VSTPLUGIN_BUNDLE_NAME "${PROJECT_NAME}.vst3")

set (VST3SDK_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/external/VST_SDK/VST3_SDK")
set (NJCLIENT_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/ninjam")

# VST plugin specific settings
include (VSTConfig)

configure_file (VstPluginDefs.h.in "${CMAKE_CURRENT_BINARY_DIR}/VstPluginDefs.h")

# Build VST3 SDK
include("cmake/Vst3.cmake")

# Build NJClient
include("../cmake/NJClient.cmake")

# Build the plugin
add_library(${VSTPLUGIN_PRJ_NAME} MODULE
    include/log.h
    include/plugcontroller.h
    include/plugids.h
    include/plugprocessor.h
    include/version.h
    include/abvst3editor.h
    include/abuimessagecontroller.h
    include/X11RunLoop.h
    include/ninjamclient.h
    include/fileutil.h
    include/stringutil.h
    include/connectionproperties.h
    include/licensedialog.h
    include/metronomevolumeparameter.h
    include/hostcontroller.h
    source/plugfactory.cpp
    source/plugcontroller.cpp
    source/plugprocessor.cpp
    source/abvst3editor.cpp
    source/X11RunLoop.cpp
    source/ninjamclient.cpp
    source/fileutil.cpp
    source/stringutil.cpp
    source/connectionproperties.cpp
    source/licensedialog.cpp
    source/metronomevolumeparameter.cpp
    source/hostcontroller.cpp)

target_include_directories(${VSTPLUGIN_PRJ_NAME}
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    PREFIX "")

plugin_add_vst3sdk(${VSTPLUGIN_PRJ_NAME})
plugin_add_vstgui(${VSTPLUGIN_PRJ_NAME})
plugin_add_njclient(${VSTPLUGIN_PRJ_NAME})

# Create the bundle (see "VST 3 Locations / Format")
execute_process (
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/plug.uidesc"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/")
    file(GLOB ABNINJAM_RESOURCE_PNG
      "${CMAKE_CURRENT_SOURCE_DIR}/resource/*.png"
    )
    file(COPY ${ABNINJAM_RESOURCE_PNG}
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
else()
    file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource/plug-base64.uidesc"
        DESTINATION "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources")
    file(RENAME "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/plug-base64.uidesc" "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/Resources/plug.uidesc")
endif()

if(WIN32)
    set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/${VST3_PACKAGE_ARCHITECTURE}-win")
else()
    set_target_properties(${VSTPLUGIN_PRJ_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}/Contents/${VST3_PACKAGE_ARCHITECTURE}-linux")
endif()


# Do not show warnings thrown by sdk
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${VSTPLUGIN_PRJ_NAME} PRIVATE
        "-Wno-extra"
        "-Wno-multichar"
        "-Wno-reorder"
        "-Wno-class-memaccess"
        "-Wno-ignored-qualifiers"
        "-Wno-unknown-pragmas"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable")
endif()

# Installation
install (DIRECTORY "${PROJECT_BINARY_DIR}/${VSTPLUGIN_BUNDLE_NAME}"
    DESTINATION "${VSTPLUGIN_INSTALL_DIR}"
    COMPONENT "vst")

