#
# Try to find AntTweakBar library and include path.
# Once done this will define
#
# ANTTWEAKBAR_FOUND
# ANTTWEAKBAR_INCLUDE_PATH
# ANTTWEAKBAR_LIBRARY
# 

IF (WIN32)
	FIND_PATH( ANTTWEAKBAR_INCLUDE_PATH AntTweakBar.h
      PATHS
		"C:/libs/AnttweakBar/include"
		$ENV{ANTTWEAKBAR_ROOT}/include
		DOC "The directory where AntTweakBar.h resides")

	if (CMAKE_CL_64)
		set (LIB_NAME AntTweakBar64.lib)
	else()
		set (LIB_NAME AntTweakBar.lib)
	endif()
    FIND_LIBRARY( ANTTWEAKBAR_LIBRARY ${LIB_NAME}
        PATHS
		"C:/libs/AnttweakBar/lib"
        $ENV{ANTTWEAKBAR_ROOT}/lib
        DOC "The AntTweakBar library")
ELSE (WIN32)
    FIND_PATH(ANT_TWEAK_BAR_INCLUDE_PATH AntTweakBar.h
      PATHS
      /usr/local/include
      /usr/X11/include
      /usr/include)

FIND_LIBRARY( ANTTWEAKBAR_LIBRARY AntTweakBar
  PATHS
    /usr/local
    /usr/X11
    /usr
  PATH_SUFFIXES
    lib64
    lib
    dylib
)
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AntTweakBar DEFAULT_MSG ANTTWEAKBAR_INCLUDE_PATH ANTTWEAKBAR_LIBRARY)
mark_as_advanced(ANTTWEAKBAR_INCLUDE_PATH ANTTWEAKBAR_LIBRARY)