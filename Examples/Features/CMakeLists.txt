
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
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${source_list})
target_link_libraries(${PROJECT_NAME} PRIVATE ImFrame)

# Set compiler options
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source" FILES ${source_list})

