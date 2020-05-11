include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to be used")
set(CMAKE_C_STANDARD 11 CACHE STRING "C standard to be used")

# Export the compile_commands.json file
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Only install what's explicitely said
set (CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Add required flags for the builds
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-ffast-math)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")
        add_compile_options(-msse2)
    endif()
    find_package(PkgConfig REQUIRED)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# If we build with Clang, optionally use libc++. Enabled by default on Apple OS.
cmake_dependent_option(USE_LIBCPP "Use libc++ with clang" "${APPLE}"
    "CMAKE_CXX_COMPILER_ID MATCHES Clang" OFF)
if (USE_LIBCPP)
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

# Default build type set as Release
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
endif()

# Show build information
function (show_build_info)
    message (STATUS "
Project name:                  ${PROJECT_NAME}
Build processor:               ${CMAKE_SYSTEM_PROCESSOR}
Use clang libc++:              ${USE_LIBCPP}

Compiler CXX debug flags:      ${CMAKE_CXX_FLAGS_DEBUG}
Compiler CXX release flags:    ${CMAKE_CXX_FLAGS_RELEASE}
Compiler CXX min size flags:   ${CMAKE_CXX_FLAGS_MINSIZEREL}
")
endfunction()

find_package(Threads REQUIRED)
