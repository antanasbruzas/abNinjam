set (VSTPLUGIN_NAME            "abNINJAM")
set (VSTPLUGIN_VENDOR          "Antanas Bruzas")
set (VSTPLUGIN_URL             "http://hippie.lt")
set (VSTPLUGIN_EMAIL           "antanas@hippie.lt")

set (VSTPLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/passengerAudio" CACHE STRING
    "Install destination for VST bundle [default: ${CMAKE_INSTALL_PREFIX}/passengerAudio}]")

message(STATUS "System architecture: ${CMAKE_SYSTEM_PROCESSOR}")

# --- VST3 Bundle architecture ---
if(NOT VST3_PACKAGE_ARCHITECTURE)
    if(APPLE)
        # VST3 packages are universal on Apple, architecture string not needed
    else()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64|x64|X64)$")
            set(VST3_PACKAGE_ARCHITECTURE "x86_64")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86|X86)$")
            if(WIN32)
                set(VST3_PACKAGE_ARCHITECTURE "x86")
            else()
                set(VST3_PACKAGE_ARCHITECTURE "i386")
            endif()
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(armv7l)$")
                set(VST3_PACKAGE_ARCHITECTURE "armv7l")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64)$")
                set(VST3_PACKAGE_ARCHITECTURE "aarch64")
        else()
            message(FATAL_ERROR "Unknown architecture for VST3: ${CMAKE_SYSTEM_PROCESSOR}.")
        endif()
    endif()
endif()

message(STATUS "VST3 architecture: ${VST3_PACKAGE_ARCHITECTURE}")

