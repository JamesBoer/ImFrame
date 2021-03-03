
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (nativefiledialog LANGUAGES CXX)

# Set the source files to a list
set(
    source_list
    "src/include/nfd.h"
	"src/common.h"
    "src/nfd_common.c"
    "src/nfd_common.h"
)	
if(WIN32)
	list(
		APPEND
		source_list
		"src/nfd_win.cpp"
	)
endif()
if(APPLE)
	list(
		APPEND
		source_list
		"nfd_cocoa.m"
	)
endif()
if(UNIX AND NOT APPLE)
	list(
		APPEND
		source_list
		"src/nfd_gtk.c"
	)
endif()

# Create executable file as project name
add_library(${PROJECT_NAME} ${source_list})

# Set compiler options
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4996)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create folder structure
foreach(source IN LISTS source_list)
    source_group("Source" FILES "${source}")
endforeach()

