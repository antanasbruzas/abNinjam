# - Try to find XKBCommon
# Once done, this will define
#
#   XKBCOMMON_FOUND - System has XKBCommon
#   XKBCOMMON_INCLUDE_DIRS - The XKBCommon include directories
#   XKBCOMMON_LIBRARIES - The libraries needed to use XKBCommon
#   XKBCOMMON_DEFINITIONS - Compiler switches required for using XKBCommon
#   XKBCOMMON_VERSION - XKBCommon version

if (XKBCOMMON_INCLUDE_DIR)
	# Already in cache, be silent
	set(XKBCOMMON_FIND_QUIETLY TRUE)
endif ()

find_package(PkgConfig QUIET)
pkg_check_modules(PC_XKBCOMMON QUIET xkbcommon)
set(XKBCOMMON_DEFINITIONS ${PC_XKBCOMMON_CFLAGS_OTHER})

set (XKBCOMMON_VERSION ${PC_XKBCOMMON_VERSION})

find_path(XKBCOMMON_INCLUDE_DIR
    NAMES xkbcommon/xkbcommon.h
    HINTS ${PC_XKBCOMMON_INCLUDE_DIR} ${PC_XKBCOMMON_INCLUDE_DIRS}
)

find_library(XKBCOMMON_LIBRARY
    NAMES xkbcommon
    HINTS ${PC_XKBCOMMON_LIBRARY} ${PC_XKBCOMMON_LIBRARY_DIRS}
)

set(XKBCOMMON_LIBRARIES ${XKBCOMMON_LIBRARY})
set(XKBCOMMON_LIBRARY_DIRS ${XKBCOMMON_LIBRARY_DIRS})
set(XKBCOMMON_INCLUDE_DIRS ${XKBCOMMON_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XKBCommon
	REQUIRED_VARS
		XKBCOMMON_LIBRARY
		XKBCOMMON_INCLUDE_DIR
	VERSION_VAR
		XKBCOMMON_VERSION
)

mark_as_advanced(XKBCOMMON_LIBRARY XKBCOMMON_INCLUDE_DIR)

