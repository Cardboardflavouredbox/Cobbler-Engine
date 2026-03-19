cmake_minimum_required(VERSION 3.10)
project(FPSthing)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.2 # Use a specific release tag
)

# Make SDL3 available to your project
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

add_executable(FPSthing src/main.cpp src/render.cpp src/input.cpp src/update.cpp src/types.cpp)

# SDL2 라이브러리 연결
target_link_libraries(FPSthing PRIVATE SDL3::SDL3 glaze::glaze)

target_include_directories(FPSthing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}/bin")
FILE(COPY MapStuff DESTINATION "${CMAKE_BINARY_DIR}/bin")