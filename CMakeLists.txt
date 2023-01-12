cmake_minimum_required(VERSION 3.7)

project(sdfddgi VERSION 1.0)
set(OpenGL_GL_PREFERENCE GLVND)

set(SOURCES
    src/main.cc
    src/Window.cc
    src/VertexArray.cc
    src/Shader.cc)

add_executable(sdfddgi ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(sdfddgi PUBLIC OpenGL::GL GLEW glfw)
target_compile_options(sdfddgi PRIVATE -fsanitize=address -fno-omit-frame-pointer -pthread)
target_link_options(sdfddgi PRIVATE -fsanitize=address)

