# - Find Cairo
#
#  CAIRO_FOUND - system has Cairo
#  CAIRO_INCLUDE_DIRS - the Cairo include directories
#  CAIRO_LIBRARIES - link these to use Cairo
#  CAIRO_VERSION - Cairo version

find_package(PkgConfig QUIET)
pkg_check_modules(PC_CAIRO QUIET cairo)

set (CAIRO_VERSION ${PC_CAIRO_VERSION})

find_path(CAIRO_INCLUDE_DIRS
    NAMES cairo.h
    HINTS ${PC_CAIRO_INCLUDEDIR}
          ${PC_CAIRO_INCLUDE_DIRS}
    PATH_SUFFIXES cairo
)

find_library(CAIRO_LIBRARIES
    NAMES cairo
    HINTS ${PC_CAIRO_LIBDIR}
          ${PC_CAIRO_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cairo
	REQUIRED_VARS
		CAIRO_LIBRARIES
		CAIRO_INCLUDE_DIRS
	VERSION_VAR
		CAIRO_VERSION
)
