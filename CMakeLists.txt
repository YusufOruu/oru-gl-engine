cmake_minimum_required(VERSION 3.10)

project(oru_gl_engine)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(oru_gl_engine
    src/main.cpp
    src/engine.cpp
)

target_link_libraries(oru_gl_engine
    OpenGL::GL
    glfw
    GLEW::GLEW
)