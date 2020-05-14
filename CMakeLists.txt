if (WIN32)
    cmake_minimum_required (VERSION 3.15)
else()
    cmake_minimum_required(VERSION 3.14.7)
endif()
project (abNinjam)
set (PROJECT_DESCRIPTION "NINJAM client")

# External configuration CMake scripts
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (abNinjamConfig)

option (ABNINJAM_VST "Enable VST plug-in build [default: ON]" ON)
option (ABNINJAM_LV2 "Enable LV2 plug-in build [default: ON]" ON)
option (ABNINJAM_USE_NJCLIENT "Use NJClient" ON)

# Add the common library targets and sources
set (COMMON_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
set (NJCLIENT_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/external/ninjam")
add_subdirectory (common)

if (ABNINJAM_VST)
    add_subdirectory (vst)
endif()

if (NOT MSVC AND NOT APPLE AND ABNINJAM_LV2)
    add_subdirectory (lv2)
endif()

# Put it at the end so that the vst directories are registered
if (NOT MSVC)
    include(abNinjamUninstall)
endif()

show_build_info()
