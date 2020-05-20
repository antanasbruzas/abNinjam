# - Find lv2
# Find the native lv2 includes
#
#  LV2_INCLUDE_DIRS - where to find lv2.h, etc.
#  LV2_FOUND        - True if lv2 found.

if (LV2_INCLUDE_DIR)
	# Already in cache, be silent
	set(LV2_FIND_QUIETLY TRUE)
endif ()

find_package (PkgConfig QUIET)
pkg_check_modules (PC_LV2 QUIET lv2)

set (LV2_VERSION ${PC_LV2_VERSION})

find_path (LV2_INCLUDE_DIR lv2.h
	HINTS
		${PC_LV2_INCLUDEDIR}
		${PC_LV2_INCLUDE_DIRS}
		${LV2_ROOT}
	)

# Handle the QUIETLY and REQUIRED arguments and set LV2_FOUND
# to TRUE if all listed variables are TRUE.
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (LV2
	REQUIRED_VARS
		LV2_INCLUDE_DIR
	VERSION_VAR
		LV2_VERSION
	)

if (LV2_FOUND)
	set (LV2_INCLUDE_DIRS ${LV2_INCLUDE_DIR})
endif ()

mark_as_advanced (LV2_INCLUDE_DIR)
