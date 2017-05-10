#
# Try to find AntTweakBar library and include path.
# Once done this will define
#
# KinectSDK_FOUND
# KinectSDK_INCLUDE_PATH
# KinectSDK_LIBRARIES
# KinectSDK_MAJOR_VERSION    Major version of the kinect sdk (1 or 2)
# 

IF (WIN32) 

    # search for kinect version 2
    #set(_paths "C:/Program Files/Microsoft SDKs/Kinect/v2.0-PublicPreview1409/")
    #
    #FIND_PATH( KinectSDK_INCLUDE_PATH Kinect.h
    #PATHS
    #    ${_paths} 
    #    PATH_SUFFIXES "inc"
    #    DOC "The directory where NuiApi.h (v1.8) or Kinect.h (v2) resides")
    #
    #if (CMAKE_CL_64)
    #    set (LIB_PREFIX "/x64/")
    #else()
    #    set (LIB_PREFIX "/x86/")
    #endif(CMAKE_CL_64)
    #FIND_LIBRARY( KinectSDK_LIBRARY Kinect20.lib
    #    PATHS
    #    ${_paths}
    #    PATH_SUFFIXES "Lib${LIB_PREFIX}"
    #    DOC "The KinectSDK library. Normally called Kinect10.lib (v1.8) or Kinect20.lib (v2)")
    #    
    #if(KinectSDK_LIBRARY)
    #    set(KinectSDK_MAJOR_VERSION 2)
    #else()
#if kinect version 2 was not found, search for version 1   
        SET(_paths "C:/Program Files/Microsoft SDKs/Kinect/v1.8/")
    
        FIND_PATH( KinectSDK_INCLUDE_PATH NuiApi.h
        PATHS
            ${_paths} 
            PATH_SUFFIXES "inc"
            DOC "The directory where NuiApi.h (v1.8) or Kinect.h (v2) resides")
    
        if(KinectSDK_INCLUDE_PATH)
            if (CMAKE_CL_64)
                set (LIB_PREFIX "/amd64/")
            else()
                set (LIB_PREFIX "/x86/")
            endif(CMAKE_CL_64)
            FIND_LIBRARY( KinectSDK_LIBRARY Kinect10.lib
                PATHS
                ${_paths}
                PATH_SUFFIXES "lib${LIB_PREFIX}"
                DOC "The KinectSDK library. Normally called Kinect10.lib")
        endif()
        
        if (KinectSDK_LIBRARY)
            set(KinectSDK_MAJOR_VERSION 1)
        endif()
    #endif()
    
else()
    message(STATUS "Kinect SDK not supported on this platform.")    
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KinectSDK DEFAULT_MSG KinectSDK_INCLUDE_PATH KinectSDK_LIBRARY)
mark_as_advanced(KinectSDK_INCLUDE_PATH KinectSDK_LIBRARY)

if (KinectSDK_MAJOR_VERSION)
    message(STATUS "Kinect SDK Version: ${KinectSDK_MAJOR_VERSION}")
endif()