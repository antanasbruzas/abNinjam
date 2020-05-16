# - Find liblo
# Find the native liblo includes and libraries
#
#  LIBLO_INCLUDE_DIRS - where to find lo.h, etc.
#  LIBLO_LIBRARIES    - List of libraries when using liblo.
#  LIBLO_FOUND        - True if liblo found.
#  LIBLO_DEFINITIONS  - Compiler switches required for using liblo

if (LIBLO_INCLUDE_DIR)
	# Already in cache, be silent
	set(LIBLO_FIND_QUIETLY TRUE)
endif ()

find_package (PkgConfig QUIET)
pkg_check_modules (PC_LIBLO QUIET liblo)

set (LIBLO_VERSION ${PC_LIBLO_VERSION})

find_path (LIBLO_INCLUDE_DIR lo/lo.h
	HINTS
		${PC_LIBLO_INCLUDEDIR}
		${PC_LIBLO_INCLUDE_DIRS}
		${LIBLO_ROOT}
	)

# The provided project files name the library with the lib prefix.
find_library (LIBLO_LIBRARY
	NAMES
		lo
		lo_static
		liblo
		liblo_static
	HINTS
		${PC_LIBLO_LIBDIR}
		${PC_LIBLO_LIBRARY_DIRS}
		${LIBLO_ROOT}
	)
# Handle the QUIETLY and REQUIRED arguments and set OGG_FOUND
# to TRUE if all listed variables are TRUE.
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Liblo
	REQUIRED_VARS
		LIBLO_LIBRARY
		LIBLO_INCLUDE_DIR
	VERSION_VAR
		LIBLO_VERSION
	)

if (LIBLO_FOUND)
	set (LIBLO_LIBRARIES ${LIBLO_LIBRARY})
	set (LIBLO_INCLUDE_DIRS ${LIBLO_INCLUDE_DIR})
endif ()

mark_as_advanced (LIBLO_INCLUDE_DIR LIBLO_LIBRARY)
