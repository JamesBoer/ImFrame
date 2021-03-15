
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project name
project(Minimal)

# Add source files and dependencies to executable
set(
	source_list
	"Main.cpp"
)
add_executable(Minimal WIN32 ${source_list})
target_link_libraries(Minimal PRIVATE ImFrame)

# Set compiler options
if(MSVC)
	target_compile_options(Minimal PRIVATE /W4 /WX)
else()
	target_compile_options(Minimal PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source" FILES ${source_list})

