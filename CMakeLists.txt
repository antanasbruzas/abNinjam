cmake_minimum_required(VERSION 3.17)
project (abNINJAM)
set (PROJECT_DESCRIPTION "NINJAM client")

# External configuration CMake scripts
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (abNINJAMConfig)

option (ABNINJAM_VST "Enable VST plug-in build [default: ON]" ON)

if (ABNINJAM_VST)
    add_subdirectory (vst)
endif()

# Put it at the end so that the vst directories are registered
if (NOT MSVC)
    include(abNINJAMUninstall)
endif()

show_build_info()
