cmake_minimum_required(VERSION 3.10)
project(Cobbler-Engine)

set(GAME "CobblerGame" CACHE STRING "Name of the Game folder")

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MINGW)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.2 
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG        release-3.2.0 
)
FetchContent_MakeAvailable(SDL3_net)

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

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(
    dylib
    GIT_REPOSITORY "https://github.com/martin-olivier/dylib"
    GIT_TAG        "v3.0.1"
)
FetchContent_MakeAvailable(dylib)

FetchContent_Declare(
	curl
	URL https://curl.se/download/curl-8.20.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE # this one
)
FetchContent_MakeAvailable(curl)

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
)

add_library(CobblerGLAD SHARED glad/glad.c "include/glad/glad.h")
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

# add_executable(Cobbler-Engine-Editor 
# src/e_main.cpp src/e_render.cpp 
# src/e_update.cpp src/e_components.cpp)

target_link_libraries(CobblerNet PRIVATE SDL3::SDL3 SDL3_net::SDL3_net CURL::libcurl)
target_link_libraries(CobblerCore PRIVATE SDL3::SDL3 glaze::glaze glm::glm freetype CobblerGLAD CobblerNet)
target_link_libraries(CobblerGameUI PRIVATE SDL3::SDL3 glm::glm freetype CobblerCore CobblerGLAD)
# target_link_libraries(CobblerEditorUI PRIVATE SDL3::SDL3 glm::glm freetype CobblerCore CobblerGLAD)
target_link_libraries(Cobbler-Engine PRIVATE SDL3::SDL3 glm::glm freetype CobblerCore CobblerGLAD dylib)
# target_link_libraries(Cobbler-Engine-Editor PRIVATE SDL3::SDL3 glm::glm freetype CobblerCore CobblerGLAD dylib)

target_include_directories(CobblerGLAD PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/UI/include)
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(CobblerEditorUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Cobbler-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(Cobbler-Engine-Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerNet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Cobbler-Engine PRIVATE DLLEXPORT=0)
target_compile_definitions(CobblerGLAD PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerGameUI PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerCore PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerNet PRIVATE DLLEXPORT=1)

FILE(COPY "$CACHE{GAME}/resources.json" DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/map DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/models DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/res DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/textures DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")

file(GLOB ENTITIES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/entities/*")
foreach(ENTITYDIR ${ENTITIES})
    cmake_path(GET ENTITYDIR FILENAME FOLDER_NAME)
    if(IS_DIRECTORY "${ENTITYDIR}")
        add_library(${FOLDER_NAME} SHARED "${ENTITYDIR}/entitymain.cpp")
        FILE(COPY "${ENTITYDIR}/sprite.bmp" DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/entities/${FOLDER_NAME}")
        set_target_properties(${FOLDER_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/entities/${FOLDER_NAME}"
        )
        set_target_properties(${FOLDER_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/entities/${FOLDER_NAME}"
        )
        target_link_libraries(${FOLDER_NAME} PRIVATE SDL3::SDL3 glm::glm CobblerCore CobblerGLAD)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()
endforeach()
