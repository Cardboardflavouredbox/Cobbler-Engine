cmake_minimum_required(VERSION 3.10)
project(FPSthing)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(FPSthing src/main.cpp src/render.cpp)

# SDL2 라이브러리 연결
target_link_libraries(FPSthing PRIVATE SDL3::SDL3)

target_include_directories(FPSthing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
