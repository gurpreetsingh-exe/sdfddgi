cmake_minimum_required(VERSION 3.7)

project(sdfddgi VERSION 1.0)
set(OpenGL_GL_PREFERENCE GLVND)

set(IMGUI_DIR vendor/imgui)

set(SOURCES
    src/main.cc
    src/Window.cc
    src/VertexArray.cc
    src/Shader.cc

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

add_executable(sdfddgi ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(sdfddgi PUBLIC OpenGL::GL GLEW glfw ${CMAKE_DL_LIBS})
target_compile_options(sdfddgi PRIVATE -fsanitize=address -fno-omit-frame-pointer -pthread)
target_link_options(sdfddgi PRIVATE -fsanitize=address)

