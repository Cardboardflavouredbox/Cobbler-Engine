cmake_minimum_required(VERSION 3.10)
project(Cobbler-Engine)

set(GAME "CobblerGame" CACHE STRING "Name of the Game folder")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MINGW)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

include(FetchContent)


find_package (Lua 5.1 REQUIRED)

find_package (sol2 CONFIG REQUIRED)

find_package (SDL3 REQUIRED)
find_package (SDL3_net REQUIRED)

FetchContent_Declare(
  bitsery
  GIT_REPOSITORY https://github.com/fraillt/bitsery.git
  GIT_TAG v5.2.5
)
FetchContent_MakeAvailable(bitsery)

find_package (Freetype REQUIRED)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

set(CORE_SOURCES 
    Core/update.cpp
    Core/components.cpp 
    Core/events.cpp
    Core/extern.cpp
    Core/files.cpp
    Core/input.cpp
    Core/physics.cpp
    Core/rendermath.cpp
    Core/entity.cpp
    Core/render.cpp
    Core/font.cpp
    Core/player.cpp
    Core/map.cpp
    Core/commandline.cpp
    Core/netconnect.cpp
    Core/particles.cpp
)

add_library(CobblerGLAD SHARED glad/gl.c "include/glad/gl.h")
add_library(CobblerCore SHARED Core/extern.cpp ${CORE_SOURCES})
add_library(CobblerGameUI SHARED "$CACHE{GAME}/UI/g_ui.cpp" "$CACHE{GAME}/UI/g_ui_index.cpp" "include/ui.h")
add_library(CobblerNet SHARED Net/net.cpp "include/network.h")
# add_library(CobblerEditorUI SHARED "$CACHE{GAME}/UI/e_ui.cpp" "$CACHE{GAME}/UI/e_ui_index.cpp" "include/ui.h")

set_target_properties(CobblerGameUI PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/bin"
)
set_target_properties(CobblerGameUI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/bin"
)

add_executable(Cobbler-Engine 
src/g_main.cpp src/g_render.cpp)

set(BSP_SOURCES 
    BSP/bsp_main.cpp
    BSP/bsp_readfile.cpp
)

add_executable(CobblerBSP ${BSP_SOURCES})

# add_executable(Cobbler-Engine-Editor 
# src/e_main.cpp src/e_render.cpp 
# src/e_update.cpp src/e_components.cpp)

target_link_libraries(CobblerNet PRIVATE SDL3::SDL3 SDL3_net::SDL3_net)
target_link_libraries(CobblerCore PRIVATE SDL3::SDL3 bitsery glm::glm Freetype::Freetype ${LUA_LIBRARIES} CobblerGLAD CobblerNet)
target_link_libraries(CobblerGameUI PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD)
# target_link_libraries(CobblerEditorUI PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD)
target_link_libraries(Cobbler-Engine PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD)
target_link_libraries(CobblerBSP PRIVATE glm::glm)
# target_link_libraries(Cobbler-Engine-Editor PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD dylib)
 
target_include_directories(CobblerGLAD PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/UI/include)
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(CobblerEditorUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Cobbler-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerBSP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerBSP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/BSP/include)
# target_include_directories(Cobbler-Engine-Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerNet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Cobbler-Engine PRIVATE DLLEXPORT=0)
target_compile_definitions(CobblerGLAD PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerGameUI PRIVATE DLLEXPORT=0 UIEXPORT=1)
target_compile_definitions(CobblerCore PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerNet PRIVATE DLLEXPORT=1)


FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION "${CMAKE_BINARY_DIR}/bin")

FILE(COPY "$CACHE{GAME}/resources.txt" DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/map DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/models DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/res DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/class DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/textures DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
