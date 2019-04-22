cmake_minimum_required(VERSION 3.9)
project(cgpg)

# Using cpp17
set(CMAKE_CXX_STANDARD 17)

# Build only dependencies of GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLFW paths
add_subdirectory(3rdparty/glfw)
include_directories(3rdparty/glfw)
include_directories(3rdparty/glfw/deps)
include_directories(3rdparty/glfw/include)

# GLFW executables.
set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
         "${GLFW_SOURCE_DIR}/deps/glad.c")
# set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h"
#            "${GLFW_SOURCE_DIR}/deps/getopt.c")

# STB stb_image, to load images
include_directories(3rdparty/stb)

# GLM library
include_directories(3rdparty/glm)

# Imgui paths
include_directories(3rdparty/imgui)
include_directories(3rdparty/imgui/examples)

# Imgui define options
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

set(IMGUI_SOURCES
    "3rdparty/imgui/imgui.cpp"
    "3rdparty/imgui/imgui_draw.cpp"
    "3rdparty/imgui/imgui_demo.cpp"
    "3rdparty/imgui/imgui_widgets.cpp"
    "3rdparty/imgui/examples/imgui_impl_glfw.cpp"
    "3rdparty/imgui/examples/imgui_impl_opengl3.cpp"
)

# Sources files
set(SOURCE_FILES src/main.cpp)

add_executable(cgpg ${SOURCE_FILES} ${GLAD} ${IMGUI_SOURCES})

target_link_libraries(cgpg glfw ${GLFW_LIBRARIES})

if(MSVC)
  target_compile_options(cgpg PRIVATE /W4 /WX)
else()
  target_compile_options(cgpg PRIVATE -Wall) # -Wextra
endif()