cmake_minimum_required (VERSION 2.8.12)
project (MT)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set (EXECUTABLE_NAME "Betris")

include(cmake//SourceGrouper.cmake)
include(SourceFiles.cmake)
include(ShaderFiles.cmake)

############################FIND PACKAGE##################
include_directories(${CMAKE_SOURCE_DIR}/common)

set(DEPENDENCIES "")

#glfw
find_package(GLFW QUIET)
if (GLFW_FOUND)
	set(_DOWNLOAD_GLFW OFF)
else()
	set(_DOWNLOAD_GLFW ON)
endif()
set(DOWNLOAD_GLFW ${_DOWNLOAD_GLFW} CACHE BOOL "Always download git from github repo. Uncheck, if you want to an external installation.")
if (${DOWNLOAD_GLFW})
	message("prepare glfw from git")
	include(ExternalProject)
	#include(LibFindMacros)
	
	message(${CMAKE_BINARY_DIR})
	set(GLFW_INSTALL_DIR "${CMAKE_BINARY_DIR}/libs/glfw")
	ExternalProject_Add(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw
		UPDATE_COMMAND ""
		CMAKE_ARGS 	-DCMAKE_INSTALL_PREFIX=${GLFW_INSTALL_DIR}
					-DGLFW_BUILD_EXAMPLES=OFF
					-DGLFW_BUILD_TESTS=OFF
					-DGLFW_BUILD_DOCS=OFF
					-DGLFW_INSTALL=ON
					-DBUILD_SHARED_LIBS=OFF
		CMAKE_CACHE_ARGS
            "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
            "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
		LOG_DOWNLOAD ON
		LOG_INSTALL OFF
    )
    
	set(GLFW_LIB_DIR "${GLFW_INSTALL_DIR}/lib")
	set(GLFW_INCLUDE_DIR "${GLFW_INSTALL_DIR}/include")
	
	
	link_directories("${GLFW_INSTALL_DIR}/lib/") #workaround: idk why, but the path is not added when encoded in GLFW_LIBRARIES
	set(GLFW_LIBRARIES "glfw3.lib") #todo: more general name
	list(APPEND DEPENDENCIES glfw)
	
	#include(git)
	#find_package(GIT)
	#execute_process()
endif()

#glew
#find_package(GLEW REQUIRED)
#include_directories(${GLEW_INCLUDE_DIR})

#boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS log log_setup thread date_time system REQUIRED)
if (NOT Boost_LIBRARY_DIR)
	message(FATAL_ERROR "Could not find Boost_LIBRARY_DIR. Please specify")
endif()
link_directories(${Boost_LIBRARY_DIR})

#ant tweakbar
find_package(AntTweakBar)#set (FORCE_QT4 ON CACHE BOOL "Use Qt4 even if Qt5 was found")
set(ANTTWEAKBAR_ON ${ANTTWEAKBAR_FOUND} CACHE BOOL "Use Ant TweakBar")

#kinect
find_package(KinectSDK)
set(USE_KINECTSDK ${KINECTSDK_FOUND} CACHE BOOL "Use the Microsoft Windows Kinect SDK")

#cg
#find_package(Cg REQUIRED)
#include_directories(${CG_INCLUDE_PATH})
#add_definitions(-DUSE_CG)

#glm
#find_package(GLM REQUIRED)
#include_directories(${GLM_INCLUDE_DIRS})

########### configure shader generator ##############
#configure shaders
set(ShaderCollectionFileName "${CMAKE_SOURCE_DIR}/Renderer/ShaderCollection.h")
file(REMOVE ${ShaderCollectionFileName})
file(WRITE ${ShaderCollectionFileName} "#include \"ShaderCollectionHeader.h\"\n")
#
foreach(_file ${shaders})
##write entries like this: 
#define FILENAME Glow #shader name
#define FILEEXT VERTEX_EXT #shader type
#include "ShaderCollectionTemplate.h"
#undef FILENAME
#undef FILEEXT
    get_filename_component(_name ${_file} NAME_WE)
    get_filename_component(_ext ${_file} EXT)
    set(_shaderType)
    if ( ${_ext} STREQUAL ".vp" )
        set(_shaderType "VERTEX_EXT")
    elseif(${_ext} STREQUAL ".fp")
        set(_shaderType "FRAGMENT_EXT")
    elseif(${_ext} STREQUAL ".geom")
        set(_shaderType "GEOMETRY_EXT")
    else()
        message("Unknown Shader Type: ${_file}")
    endif()
    file(READ ${_file} _isNotEmpty)
    if (_shaderType AND _isNotEmpty)
        file(APPEND ${ShaderCollectionFileName} "#define FILENAME ${_name}\n")
        file(APPEND ${ShaderCollectionFileName} "#define FILEEXT ${_shaderType}\n#include\"ShaderCollectionTemplate.h\"\n#undef FILENAME\n#undef FILEEXT\n")
    endif()
endforeach()


add_executable(${EXECUTABLE_NAME} ${sources} ${shaders} ${ShaderCollectionFileName})
add_dependencies(${EXECUTABLE_NAME} ${DEPENDENCIES})
group_target_sources(${EXECUTABLE_NAME})
######################LINK LIBS#########################
#boost
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries( ${EXECUTABLE_NAME} ${Boost_LIBRARIES} )

#glfw
message(${GLFW_INCLUDE_DIR})
target_include_directories( ${EXECUTABLE_NAME} PUBLIC ${GLFW_INCLUDE_DIR})
add_definitions(-DTIME_MEASURE)
target_link_libraries(${EXECUTABLE_NAME} ${GLFW_LIBRARIES})

#anttweakbar
if(${ANTTWEAKBAR_ON})
	target_include_directories(${EXECUTABLE_NAME} PUBLIC ${ANTTWEAKBAR_INCLUDE_PATH})
	target_link_libraries(${EXECUTABLE_NAME} ${ANTTWEAKBAR_LIBRARY})
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES COMPILE_DEFINITIONS $<$<CONFIG:Debug>:USE_ANTTWEAKBAR>)
endif(${ANTTWEAKBAR_ON})

#kinect
if(${USE_KINECTSDK})
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${KinectSDK_INCLUDE_PATH})
    target_link_libraries(${EXECUTABLE_NAME} ${KinectSDK_LIBRARY})
    add_definitions(-DUSE_KINECTSDK)
    add_definitions(-DKINECT_SDK_VERSION=${KinectSDK_MAJOR_VERSION})
endif()

#opengl
if (WIN32)
  target_link_libraries( ${EXECUTABLE_NAME} "opengl32.lib")
else()
  target_link_libraries( ${EXECUTABLE_NAME} "GL" "X11" "pthread")
endif()


#target_link_libraries( ${EXECUTABLE_NAME} ${GLEW_LIBRARY})
#target_link_libraries( ${EXECUTABLE_NAME} ${CG_LIBRARY})
#target_link_libraries( ${EXECUTABLE_NAME} ${CG_GL_LIBRARY})

if(MSVC)
	add_definitions(
		-D_SCL_SECURE_NO_WARNINGS
		-DNOMINMAX
		)
endif ()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=gnu++11)
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wno-write-strings")
endif()

################ ADD SHADERS ###############
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -P "$<$<NOT:$<CONFIG:DEBUG>>:cmake/Prebuild.cmake>" "$<$<CONFIG:Debug>:cmake/Empty.cmake>"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -P "$<$<NOT:$<CONFIG:DEBUG>>:cmake/Postbuild.cmake>" "$<$<CONFIG:Debug>:cmake/Empty.cmake>"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

