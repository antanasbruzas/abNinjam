# Add ninjam -------

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

if (CMAKE_FIND_PACKAGE_PREFER_CONFIG)
    message(STATUS "CMAKE_FIND_PACKAGE_PREFER_CONFIG: " ${CMAKE_FIND_PACKAGE_PREFER_CONFIG})
endif()

find_package(Vorbis COMPONENTS Enc REQUIRED)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "OGG_INCLUDE_DIRS:" ${OGG_INCLUDE_DIRS})
    message(STATUS "OGG_LIBRARIES:" ${OGG_LIBRARIES})
    message(STATUS "VORBIS_INCLUDE_DIRS:" ${VORBIS_INCLUDE_DIRS})
    message(STATUS "VORBIS_LIBRARIES:" ${VORBIS_LIBRARIES})
    message(STATUS "Vorbis_Vorbis_INCLUDE_DIRS:" ${Vorbis_Vorbis_INCLUDE_DIRS})
    message(STATUS "Vorbis_Vorbis_LIBRARIES:" ${Vorbis_Vorbis_LIBRARIES})
    message(STATUS "Vorbis_Enc_INCLUDE_DIRS:" ${Vorbis_Enc_INCLUDE_DIRS})
    message(STATUS "Vorbis_Enc_LIBRARIES:" ${Vorbis_Enc_LIBRARIES})
    message(STATUS "Vorbis_File_INCLUDE_DIRS:" ${Vorbis_File_INCLUDE_DIRS})
    message(STATUS "Vorbis_File_LIBRARIES:" ${Vorbis_File_LIBRARIES})
endif()

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

#unset(CMAKE_FIND_LIBRARY_SUFFIXES)
