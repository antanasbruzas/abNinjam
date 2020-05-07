# Add ninjam -------
# Find vorbisenc dependency
if(WIN32)
    find_package(Ogg REQUIRED)
    find_package(Vorbis REQUIRED)
    find_package(VorbisEnc REQUIRED)
else()
    pkg_check_modules(OGG ogg>=1.3.0)
    pkg_check_modules(VORBIS vorbis>=1.3.3)
    pkg_check_modules(VORBISENC vorbisenc>=1.3.3)
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

    target_include_directories("${NAME}" PRIVATE "${NJCLIENT_BASEDIR}")
    target_link_libraries("${NAME}" PRIVATE ${OGG_LIBRARIES})
    target_link_libraries("${NAME}" PRIVATE ${VORBIS_LIBRARIES})
    target_link_libraries("${NAME}" PRIVATE ${VORBISENC_LIBRARIES})
endfunction()

