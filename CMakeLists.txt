if (WIN32)
    cmake_minimum_required (VERSION 3.15)
    cmake_policy(SET CMP0091 NEW)
else()
    cmake_minimum_required(VERSION 3.14.7)
endif()
project (abNinjam)
set (PROJECT_DESCRIPTION "NINJAM client")

# External configuration CMake scripts
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (abNinjamConfig)

option (ABNINJAM_VST "Enable VST plug-in build [default: ON]" ON)

if (ABNINJAM_VST)
    add_subdirectory (vst)
endif()

# Put it at the end so that the vst directories are registered
if (NOT MSVC)
    include(abNinjamUninstall)
endif()

show_build_info()
