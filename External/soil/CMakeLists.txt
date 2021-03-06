
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (soil LANGUAGES CXX)

# Set the source files to a list
set(
    include_list
    "include/SOIL/image_dxt.h"
    "include/SOIL/image_helper.h"
    "include/SOIL/SOIL.h"
    "include/SOIL/stb_image_aug.h"
    "include/SOIL/stbi_dds_aug.h"
    "include/SOIL/stbi_dds_aug_c.h"
)	
set(
    source_list
    "src/image_dxt.c"
    "src/image_helper.c"
    "src/soil.c"
    "src/stb_image_aug.c"
)	

# Create library from source files
add_library(${PROJECT_NAME} ${include_list} ${source_list})
if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32.lib)
endif()

# Set include directory
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Create folder structure
foreach(include IN LISTS include_list)
    source_group("Include" FILES "${include}")
endforeach()

foreach(source IN LISTS source_list)
    source_group("Source" FILES "${source}")
endforeach()



