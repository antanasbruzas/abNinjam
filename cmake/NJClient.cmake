# Add ninjam -------
if (NOT CMAKE_VERSION VERSION_LESS 3.15)
        set (CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
endif ()

if (CMAKE_FIND_PACKAGE_PREFER_CONFIG)
    message(STATUS "CMAKE_FIND_PACKAGE_PREFER_CONFIG: " ${CMAKE_FIND_PACKAGE_PREFER_CONFIG})
#    find_package (Ogg CONFIG REQUIRED)
#    find_package (Vorbis CONFIG COMPONENTS Enc REQUIRED)
#    include (FindPackageHandleStandardArgs)
#    find_package_handle_standard_args (Ogg CONFIG_MODE)
#    find_package_handle_standard_args (Vorbis CONFIG_MODE)
#else ()
#    find_package (Ogg REQUIRED)
#    find_package (Vorbis COMPONENTS Enc REQUIRED)
endif ()

find_package (Ogg REQUIRED)
find_package (Vorbis COMPONENTS Enc REQUIRED)

# --- NJClient ---
function(plugin_add_njclient NAME)
    target_sources("${NAME}" PRIVATE
        "${NJCLIENT_BASEDIR}/ninjam/njclient.cpp"
        "${NJCLIENT_BASEDIR}/ninjam/netmsg.cpp"
        "${NJCLIENT_BASEDIR}/ninjam/mpb.cpp"
        "${NJCLIENT_BASEDIR}/WDL/rng.cpp"
        "${NJCLIENT_BASEDIR}/WDL/sha.cpp"
        "${NJCLIENT_BASEDIR}/WDL/jnetlib/asyncdns.cpp"
        "${NJCLIENT_BASEDIR}/WDL/jnetlib/connection.cpp"
        "${NJCLIENT_BASEDIR}/WDL/jnetlib/util.cpp")

    if(WIN32)
         target_sources("${NAME}" PRIVATE
            "${NJCLIENT_BASEDIR}/WDL/win32_utf8.c")
        target_link_libraries("${NAME}" PRIVATE ws2_32)
    endif()

    target_include_directories("${NAME}" PRIVATE "${NJCLIENT_BASEDIR}")
    target_include_directories("${NAME}" PRIVATE "${OGG_INCLUDE_DIRS}")
    target_include_directories("${NAME}" PRIVATE "${Vorbis_Vorbis_INCLUDE_DIRS}")
    target_include_directories("${NAME}" PRIVATE "${Vorbis_Enc_INCLUDE_DIRS}")
    target_link_libraries("${NAME}" PRIVATE ${OGG_LIBRARIES})
    target_link_libraries("${NAME}" PRIVATE ${Vorbis_Vorbis_LIBRARIES})
    target_link_libraries("${NAME}" PRIVATE ${Vorbis_Enc_LIBRARIES})
endfunction()
