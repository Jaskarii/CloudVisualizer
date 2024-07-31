cmake_minimum_required(VERSION 3.8)

project(MyOpenGL)
set(CMAKE_CXX_STANDARD 17)
if(WIN32)
    set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/GLFW)
    set(GLFW_LIB_DIR ${CMAKE_SOURCE_DIR}/GLFW)
    add_compile_definitions(WINVER=0x0A00 _WIN32_WINNT=0x0A00)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    link_directories(${GLFW_LIB_DIR})
else()
    find_package(glfw3 REQUIRED)
endif()

find_package(Threads REQUIRED)

include_directories(${GLFW_INCLUDE_DIR})
include_directories(
    include 
    headers
)

set(CMAKE_BUILD_TYPE "Debug")

add_library(glad glad.c)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Copy shader files to the build directory
file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Gather all ImGui source files
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/headers/ImGui/*.cpp)
file(GLOB SOURCES "src/*.cpp")
file(GLOB SHADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/UDPSocket.h)

if(WIN32)
    list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/win32/UDPSocket_Win.cpp)
else()
    list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Linux/UDPSocket_Ubuntu.cpp)
endif()

add_executable(MyOpenGLApp ${SOURCES} ${IMGUI_SOURCES} ${SOURCE_FILES})

target_link_libraries(MyOpenGLApp
    glad  # Example for linking against OpenGL
    glfw
    #ws2_32
    dl
    Threads::Threads
)