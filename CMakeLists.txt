
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

# Ensure ImGui integration file uses glad loader
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

# Set some specific glfw options
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

# Add external libraries
add_subdirectory("External/glad")
add_subdirectory("External/glfw")
add_subdirectory("External/Imgui")
add_subdirectory("External/Implot")
add_subdirectory("External/nativefiledialog")
add_subdirectory("External/soil")
add_custom_target(linmath SOURCES "External/linmath/linmath.h")
add_custom_target(mINI SOURCES "External/mINI/src/mini/ini.h")

# Indicate that we should place these in the External project folder
set_property(TARGET glad PROPERTY FOLDER "External")
set_property(TARGET glfw PROPERTY FOLDER "External")
set_property(TARGET Imgui PROPERTY FOLDER "External")
set_property(TARGET Implot PROPERTY FOLDER "External")
set_property(TARGET nativefiledialog PROPERTY FOLDER "External")
set_property(TARGET linmath PROPERTY FOLDER "External")
set_property(TARGET mINI PROPERTY FOLDER "External")
set_property(TARGET soil PROPERTY FOLDER "External")

# Add source files and dependencies to library
set(
	source_list
	"Include/ImFrame.h"
	"Source/ImfInternal.h"
	"Source/ImFrame.cpp"
	"Source/ImfUtilities.cpp"
	"Source/ImfUtilities.h"
	"Source/Fonts/CarlitoRegular.h"
	"Source/Fonts/OpenSansRegular.h"
	"Source/Fonts/OpenSansSemiBold.h"
	"Source/Fonts/RobotoMedium.h"
	"Source/Fonts/RobotoRegular.h"
	"Source/ImGui/imgui_impl_glfw.cpp"
	"Source/ImGui/imgui_impl_glfw.h"
	"Source/ImGui/imgui_impl_opengl3.cpp"
	"Source/ImGui/imgui_impl_opengl3.h"
)
if(WIN32)
	list(APPEND source_list "Source/WinPC/WinPCUtilities.cpp")
elseif(APPLE)
	list(APPEND source_list "Source/macOS/macOSUtilities.mm")
elseif(UNIX AND NOT APPLE)
	list(APPEND source_list "Source/Linux/LinuxUtilities.cpp")
endif()
add_library(ImFrame ${source_list})
target_link_libraries(ImFrame PRIVATE glad glfw Imgui Implot nativefiledialog soil)

# Set compiler options
if(MSVC)
	target_compile_options(ImFrame PRIVATE /W4 /WX)
else()
	target_compile_options(ImFrame PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Indicate various header includes for projects
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/Include")
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/External/glad/include")
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/External/glfw/include")
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/External/Imgui")
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/External/Implot")
target_include_directories(ImFrame PUBLIC "${PROJECT_SOURCE_DIR}/External/linmath")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/nativefiledialog/src/include")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/mINI/src")
target_include_directories(ImFrame PRIVATE "${PROJECT_SOURCE_DIR}/External/soil/include")
target_include_directories(Implot PRIVATE "${PROJECT_SOURCE_DIR}/External/Imgui")
target_include_directories(soil PRIVATE "${PROJECT_SOURCE_DIR}/External/glad/include")

# Create folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_list})

# Check if Imframe is included via add_subdirectory.
get_directory_property(imframe_is_subproject PARENT_DIRECTORY)

# Only build the test suite and utilities if this is not a subproject
if(NOT imframe_is_subproject)
	add_subdirectory("Examples/Features")
	add_subdirectory("Examples/Minimal")
	set_property(TARGET Features PROPERTY FOLDER "Examples")
	set_property(TARGET Minimal PROPERTY FOLDER "Examples")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Features)
endif()
