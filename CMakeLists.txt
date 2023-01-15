cmake_minimum_required(VERSION 3.7)

project(sdfddgi VERSION 1.0)
set(OpenGL_GL_PREFERENCE GLVND)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(IMGUI_DIR vendor/imgui)

set(SOURCES
    src/main.cc
    src/Window.cc
    src/Shader.cc
    src/Camera.cc
    src/Mesh.cc

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_subdirectory(vendor/tinygltf)

include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

add_executable(sdfddgi ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(sdfddgi PUBLIC OpenGL::GL GLEW glfw tinygltf ${CMAKE_DL_LIBS})
# target_compile_options(sdfddgi PRIVATE -ggdb -fsanitize=address -fno-omit-frame-pointer -pthread)
# target_link_options(sdfddgi PRIVATE -fsanitize=address)

