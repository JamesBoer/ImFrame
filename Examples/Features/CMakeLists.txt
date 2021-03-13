
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project name
project(Features)

# Add source files and dependencies to executable
set(
	source_list
	"GLDemo.cpp"
	"GLDemo.h"
	"Internal.h"
	"Main.cpp"
	"MainApp.cpp"
	"MainApp.h"
	"SetBgColor.cpp"
	"SetBgColor.h"
	"SetUiFont.cpp"
	"SetUiFont.h"
)
add_executable(Features WIN32 MACOSX_BUNDLE ${source_list})
target_link_libraries(Features PRIVATE ImFrame)

# Set compiler options
if(MSVC)
	target_compile_options(Features PRIVATE /W4 /WX)
else()
	target_compile_options(Features PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Copy icon file from source to binary output folder
if(MSVC)
	add_custom_command(
		TARGET Features POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_SOURCE_DIR}/Data/Icons/world.png
			${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/world.png
	)
elseif(UNIX AND NOT APPLE)
	add_custom_command(
		TARGET Features POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_SOURCE_DIR}/Data/Icons/world.png
			${CMAKE_CURRENT_BINARY_DIR}/world.png
	)
endif()

# Create folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source" FILES ${source_list})

