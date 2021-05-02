
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set project name
project(stb)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files and dependencies to library
set(
    source_list
	"stb_image.cpp"
	"stb_image.h"
	"stb_image_resize.cpp"
	"stb_image_resize.h"
)

add_library(${PROJECT_NAME} ${source_list})
target_link_libraries(${PROJECT_NAME})

# Create folder structure
source_group("Source" FILES ${source_list})


