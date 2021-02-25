
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (Imgui LANGUAGES CXX)

# Set the source files to a list
set(
    source_list
    "imgui.cpp"
	"imgui.h"
	"imgui_demo.cpp"
	"imgui_draw.cpp"
	"imgui_widgets.cpp"
	"imgui_internal.h"
	"imconfig.h"
	"imstb_rectpack.h"
	"imstb_textedit.h"
	"imstb_truetype.h"
)	

# Create executable file as project name
add_library(${PROJECT_NAME} ${source_list})

# Create folder structure
foreach(source IN LISTS source_list)
    source_group("Source" FILES "${source}")
endforeach()



