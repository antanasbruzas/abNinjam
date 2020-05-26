# Common sources
set (COMMON_SOURCES
    "${COMMON_BASEDIR}/filesystem.hpp"
    "${COMMON_BASEDIR}/common.h"
    "${COMMON_BASEDIR}/include/log.h"
    "${COMMON_BASEDIR}/include/ninjamclient.h"
    "${COMMON_BASEDIR}/include/fileutil.h"
    "${COMMON_BASEDIR}/include/stringutil.h"
    "${COMMON_BASEDIR}/include/connectionproperties.h"
    "${COMMON_BASEDIR}/include/licensedialog.h"
    "${COMMON_BASEDIR}/include/ninjamclientstatus.h"
    "${COMMON_BASEDIR}/include/osctransmitter.h"
    "${COMMON_BASEDIR}/include/remotechannel.h"
    "${COMMON_BASEDIR}/include/remoteuser.h"
    "${COMMON_BASEDIR}/include/remoteuserchannel.h"
    "${COMMON_BASEDIR}/src/ninjamclient.cpp"
    "${COMMON_BASEDIR}/src/fileutil.cpp"
    "${COMMON_BASEDIR}/src/stringutil.cpp"
    "${COMMON_BASEDIR}/src/connectionproperties.cpp"
    "${COMMON_BASEDIR}/src/licensedialog.cpp"
    "${COMMON_BASEDIR}/src/osctransmitter.cpp"
    "${COMMON_BASEDIR}/src/remotechannel.cpp"
    "${COMMON_BASEDIR}/src/remoteuser.cpp"
    "${COMMON_BASEDIR}/src/remoteuserchannel.cpp")

set (COMMON_PRJ_NAME "${PROJECT_NAME}Common")
add_library(${COMMON_PRJ_NAME} STATIC ${COMMON_SOURCES})
target_include_directories (${COMMON_PRJ_NAME} PUBLIC .)
target_include_directories (${COMMON_PRJ_NAME} PUBLIC external)
set_target_properties (${COMMON_PRJ_NAME} PROPERTIES OUTPUT_NAME ${COMMON_PRJ_NAME} PUBLIC_HEADER "common.h")

if (ABNINJAM_USE_LIBLO)
    find_package(Liblo REQUIRED)
    target_include_directories(${COMMON_PRJ_NAME} PUBLIC ${LIBLO_INCLUDE_DIRS})
    target_link_libraries(${COMMON_PRJ_NAME} PRIVATE ${LIBLO_LIBRARIES})
    if (MSVC)
        # link missing libraries for liblo_static.lib
        target_link_libraries(${COMMON_PRJ_NAME} PRIVATE ws2_32 iphlpapi)
    endif()
    add_definitions(-DABNINJAM_USE_LIBLO=${ABNINJAM_USE_LIBLO})
    message(STATUS "ABNINJAM_USE_LIBLO: ${ABNINJAM_USE_LIBLO}")
endif()
target_include_directories(${COMMON_PRJ_NAME} PRIVATE "${COMMON_BASEDIR}")
if (ABNINJAM_USE_NJCLIENT)
    # Build NJClient
    include("../cmake/NJClient.cmake")
    plugin_add_njclient(${COMMON_PRJ_NAME})
    if (${CMAKE_BUILD_TYPE} MATCHES "Release")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${COMMON_PRJ_NAME} PRIVATE
                "-Wno-unused-function"
                "-Wno-unused-parameter"
                "-Wno-reorder"
                "-Wno-unused-function"
                "-Wno-unused-parameter"
                "-Wno-missing-field-initializers"
                "-Wno-cast-function-type"
                "-Wno-maybe-uninitialized")
        endif()
    endif()
endif()

add_library (${PROJECT_NAME}::common ALIAS ${COMMON_PRJ_NAME})






