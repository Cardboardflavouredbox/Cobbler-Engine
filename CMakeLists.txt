cmake_minimum_required(VERSION 3.10)
project(Cobbler-Engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.2 # Use a specific release tag
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-14-3
)
FetchContent_MakeAvailable(freetype)


add_executable(Cobbler-Engine src/gamemain.cpp src/render.cpp src/input.cpp src/update.cpp src/types.cpp src/files.cpp src/events.cpp src/ui.cpp src/components.cpp)

add_executable(Cobbler-Engine-Editor src/editormain.cpp src/render.cpp src/input.cpp src/editorupdate.cpp src/types.cpp src/files.cpp src/events.cpp src/ui.cpp src/components.cpp)

target_link_libraries(Cobbler-Engine PRIVATE SDL3::SDL3 glaze::glaze freetype)
target_link_libraries(Cobbler-Engine-Editor PRIVATE SDL3::SDL3 glaze::glaze freetype)

target_include_directories(Cobbler-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Cobbler-Engine-Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}/bin")
FILE(COPY MapStuff DESTINATION "${CMAKE_BINARY_DIR}/bin")