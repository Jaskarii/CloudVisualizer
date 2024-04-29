cmake_minimum_required(VERSION 3.8)

project(MyOpenGL)
set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED) # Add this line


include_directories(${GLFW_INCLUDE_DIR})  # This line is optional, but it's good practice
include_directories(
    include 
    headers
)

add_library(glad glad.c)

# Gather all ImGui source files
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/headers/ImGui/*.cpp)
file(GLOB SOURCES "src/*.cpp")
file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_executable(MyOpenGLApp ${SOURCES} ${IMGUI_SOURCES})

target_link_libraries(MyOpenGLApp
    glad  # Example for linking against OpenGL
    glfw
    dl
    Threads::Threads
)