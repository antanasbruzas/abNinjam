# Common sources
set (COMMON_SOURCES
    "${COMMON_BASEDIR}/common.h"
    "${COMMON_BASEDIR}/include/log.h"
    "${COMMON_BASEDIR}/include/ninjamclient.h"
    "${COMMON_BASEDIR}/include/fileutil.h"
    "${COMMON_BASEDIR}/include/stringutil.h"
    "${COMMON_BASEDIR}/include/connectionproperties.h"
    "${COMMON_BASEDIR}/include/licensedialog.h"
    "${COMMON_BASEDIR}/include/ninjamclientstatus.h"
    "${COMMON_BASEDIR}/include/osctransmitter.h"
    "${COMMON_BASEDIR}/source/ninjamclient.cpp"
    "${COMMON_BASEDIR}/source/fileutil.cpp"
    "${COMMON_BASEDIR}/source/stringutil.cpp"
    "${COMMON_BASEDIR}/source/connectionproperties.cpp"
    "${COMMON_BASEDIR}/source/licensedialog.cpp"
    "${COMMON_BASEDIR}/source/osctransmitter.cpp")


if (ABNINJAM_USE_LIBLO)
   find_package(Liblo REQUIRED)
endif()

# Add common files for developing
if (UNIX AND NOT APPLE)
    set (COMMON_PRJ_NAME "${PROJECT_NAME}_common")
    add_library(${COMMON_PRJ_NAME} MODULE ${COMMON_SOURCES})
    add_library (${PROJECT_NAME}::common ALIAS ${COMMON_PRJ_NAME})
    if (${CMAKE_BUILD_TYPE} MATCHES "Release")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${COMMON_PRJ_NAME} PRIVATE
                "-Wno-unused-function"
                "-Wno-unused-parameter")
        endif()
    endif()
endif()

# --- Common ---
function(plugin_add_common NAME)
    target_sources("${NAME}" PRIVATE
        "${COMMON_BASEDIR}/common.h"
        "${COMMON_BASEDIR}/include/log.h"
        "${COMMON_BASEDIR}/include/ninjamclient.h"
        "${COMMON_BASEDIR}/include/fileutil.h"
        "${COMMON_BASEDIR}/include/stringutil.h"
        "${COMMON_BASEDIR}/include/connectionproperties.h"
        "${COMMON_BASEDIR}/include/licensedialog.h"
        "${COMMON_BASEDIR}/include/ninjamclientstatus.h"
        "${COMMON_BASEDIR}/include/osctransmitter.h"
        "${COMMON_BASEDIR}/source/ninjamclient.cpp"
        "${COMMON_BASEDIR}/source/fileutil.cpp"
        "${COMMON_BASEDIR}/source/stringutil.cpp"
        "${COMMON_BASEDIR}/source/connectionproperties.cpp"
        "${COMMON_BASEDIR}/source/licensedialog.cpp"
        "${COMMON_BASEDIR}/source/osctransmitter.cpp")
    if (ABNINJAM_USE_LIBLO)
        target_include_directories("${NAME}" PRIVATE ${LIBLO_INCLUDE_DIRS})
        target_link_libraries("${NAME}" PRIVATE ${LIBLO_LIBRARIES})
    endif()
    target_include_directories("${NAME}" PRIVATE "${COMMON_BASEDIR}")
    if (ABNINJAM_USE_NJCLIENT)
        # Build NJClient
        include("../cmake/NJClient.cmake")
        plugin_add_njclient(${NAME})
    endif()
endfunction()






