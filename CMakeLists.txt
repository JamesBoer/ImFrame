
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project name
project(ImFrame)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define UNICODE for MSVC
if(MSVC)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)
endif()

# Set some specific glfw options
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

# Add external libraries
add_subdirectory("External/glad")
add_subdirectory("External/glfw")
add_subdirectory("External/Imgui")
add_subdirectory("External/Implot")

# Indicate that we should place these in the External project folder
set_property(TARGET glad PROPERTY FOLDER "External")
set_property(TARGET glfw PROPERTY FOLDER "External")
set_property(TARGET Imgui PROPERTY FOLDER "External")
set_property(TARGET Implot PROPERTY FOLDER "External")

# Add source files and dependencies to library
set(
	source_list
	"Source/ImFrame.cpp"
	"Source/ImFrame.h"
	"Source/ImFrameInternal.h"
)
add_library(ImFrame ${source_list})
target_link_libraries(ImFrame PRIVATE glad glfw Imgui Implot)

# Set compiler options
if(MSVC)
	target_compile_options(ImFrame PRIVATE /W4 /WX)
else()
	target_compile_options(ImFrame PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Indicate various header includes for projects
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/glad/include")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/glfw/include")
target_include_directories(Implot PRIVATE "${PROJECT_SOURCE_DIR}/External/Imgui")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/Imgui")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/Implot")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/linmath")

# Check if Imframe is included via add_subdirectory.
get_directory_property(imframe_is_subproject PARENT_DIRECTORY)

# Only build the test suite and utilities if this is not a subproject
if(NOT imframe_is_subproject)
	add_executable(Feature WIN32 "Tests/Feature/Feature.cpp")
	set_property(TARGET Feature PROPERTY WIN32_EXECUTABLE TRUE)
	target_link_libraries(Feature ImFrame)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Feature)
endif()

# Create folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_list})


