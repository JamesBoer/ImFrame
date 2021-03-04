
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

# GTK specific section for Linux build
if(UNIX AND NOT APPLE)

	# Use the package PkgConfig to detect GTK+ headers/library files
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

	# Setup CMake to use GTK+, tell the compiler where to look for headers
	# and to the linker where to look for libraries
	include_directories(${GTK3_INCLUDE_DIRS})
	link_directories(${GTK3_LIBRARY_DIRS})

	# Add other flags to the compiler
	add_definitions(${GTK3_CFLAGS_OTHER})
endif()

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
		"src/nfd_cocoa.m"
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
endif()
if(APPLE)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()
if(UNIX AND NOT APPLE)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wno-stringop-truncation -Wno-format-truncation)
endif()

# GTK specific section for Linux build
if(UNIX AND NOT APPLE)

	# Link the target to the GTK+ libraries
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTK3_LIBRARIES})

endif()

# Create folder structure
foreach(source IN LISTS source_list)
    source_group("Source" FILES "${source}")
endforeach()

