set (LV2PLUGIN_PRJ_NAME "${PROJECT_NAME}_lv2")

# Set the build directory as <build_dir>/lv2/<plugin_name>.lv2/
set (PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lv2")

# LV2 plugin specific settings
include (LV2Config)

# Keep non build turtle files in IDE
set (LV2PLUGIN_TTL_SRC_FILES
    manifest.ttl.in
    ${PROJECT_NAME}.ttl.in
)
# Build the plugin
add_library (${LV2PLUGIN_PRJ_NAME} MODULE
    include/ports.h
    source/${PROJECT_NAME}.cpp
    ${LV2PLUGIN_TTL_SRC_FILES})


if(WIN32)
    find_package(LV2 CONFIG REQUIRED)
else()
    pkg_check_modules(LV2 lv2>=1.16.0)
endif()
target_include_directories("${LV2PLUGIN_PRJ_NAME}" PRIVATE ${LV2_INCLUDE_DIRS})
target_link_libraries("${LV2PLUGIN_PRJ_NAME}" PRIVATE ${LV2_LIBRARIES})

# Remove the "lib" prefix, rename the target name and build it in the .lv2 build dir
# <build_dir>/lv2/<plugin_name>_lv2.<ext> to
# <build_dir>/lv2/<plugin_name>.lv2/<plugin_name>.<ext>
set_target_properties (${LV2PLUGIN_PRJ_NAME} PROPERTIES PREFIX "")
set_target_properties (${LV2PLUGIN_PRJ_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties (${LV2PLUGIN_PRJ_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

if (UNIX AND NOT APPLE)
    add_dependencies(${LV2PLUGIN_PRJ_NAME} abNinjam::common)
endif()
plugin_add_common(${LV2PLUGIN_PRJ_NAME})

# Generate *.ttl files from *.in sources,
# create the destination directory if it doesn't exists and copy needed files
file (MAKE_DIRECTORY ${PROJECT_BINARY_DIR})
configure_file (manifest.ttl.in ${PROJECT_BINARY_DIR}/manifest.ttl)
configure_file (${PROJECT_NAME}.ttl.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.ttl)
configure_file (LICENSE.md.in ${PROJECT_BINARY_DIR}/LICENSE.md)
if (ABNINJAM_USE_NJCLIENT)
    file(COPY "gpl-2.0.txt" DESTINATION ${PROJECT_BINARY_DIR})
endif()

# Installation
if (NOT MSVC)
    install (DIRECTORY ${PROJECT_BINARY_DIR} DESTINATION ${LV2PLUGIN_INSTALL_DIR}
        COMPONENT "lv2")
endif()
