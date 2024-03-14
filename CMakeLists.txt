# CMakeList.txt : CMake project for LayerMatNode, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

project("LayerMatNode")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/plugin)

# Enable C++11 for host code
set(CMAKE_CXX_STANDARD 17)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(ARNOLD_DIR "C:\\Program\ Files/Autodesk/Arnold/maya2023")
include_directories("${ARNOLD_DIR}/include/arnold")

file(GLOB_RECURSE core_headers
	"${CMAKE_SOURCE_DIR}/src/*.h"
	"${CMAKE_SOURCE_DIR}/src/*.hpp")

file(GLOB_RECURSE core_sources
	"${CMAKE_SOURCE_DIR}/src/*.c"
	"${CMAKE_SOURCE_DIR}/src/*.cpp")

if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	GroupSources(src)
endif()

# Add source to this project's executable.
add_library(${CMAKE_PROJECT_NAME} SHARED ${core_headers} ${core_sources})

target_link_libraries(${CMAKE_PROJECT_NAME} "${ARNOLD_DIR}/lib/ai.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


# Directory containing the built .py files

# Destination directory to copy .py files to
set(DEST_PY_DIR ${ARNOLD_DIR})

# Add a custom command that runs after building 'MyApp' target
# This assumes that your build process somehow produces .py files in SOURCE_PY_DIR

add_custom_command(TARGET LayerMatNode POST_BUILD
    COMMAND ${CMAKE_COMMAND} -D DEST_ROOT:STRING=${DEST_PY_DIR} -D SOURCE_DIR:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} -P ${CMAKE_SOURCE_DIR}/copyFileCMake.cmake
    COMMENT "Copying .py files to ${DEST_PY_DIR}"
)

# TODO: Add tests and install targets if needed.
