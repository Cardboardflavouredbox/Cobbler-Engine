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


FetchContent_Declare(
    dylib
    GIT_REPOSITORY "https://github.com/martin-olivier/dylib"
    GIT_TAG        "v3.0.1"
)
FetchContent_MakeAvailable(dylib)

find_package (Vulkan REQUIRED)

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

target_link_libraries(CobblerNet PRIVATE SDL3::SDL3 SDL3_net::SDL3_net)
target_link_libraries(CobblerCore PRIVATE SDL3::SDL3 bitsery glm::glm Freetype::Freetype CobblerGLAD CobblerNet Vulkan::Vulkan)
target_link_libraries(CobblerGameUI PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD)
# target_link_libraries(CobblerEditorUI PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD)
target_link_libraries(Cobbler-Engine PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD dylib)
# target_link_libraries(Cobbler-Engine-Editor PRIVATE SDL3::SDL3 glm::glm Freetype::Freetype CobblerCore CobblerGLAD dylib)
 
target_include_directories(CobblerGLAD PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerCore PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/UI/include)
target_include_directories(CobblerGameUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(CobblerEditorUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Cobbler-Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(Cobbler-Engine-Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(CobblerNet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Cobbler-Engine PRIVATE DLLEXPORT=0)
target_compile_definitions(CobblerGLAD PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerGameUI PRIVATE DLLEXPORT=0 UIEXPORT=1)
target_compile_definitions(CobblerCore PRIVATE DLLEXPORT=1)
target_compile_definitions(CobblerNet PRIVATE DLLEXPORT=1)

FILE(COPY "$CACHE{GAME}/resources.txt" DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/map DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/models DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/res DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")
FILE(COPY $CACHE{GAME}/textures DESTINATION "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}")

file(GLOB ENTITIES LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/entities/*")
file(GLOB ENTITIES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/entities/*")
foreach(ENTITYDIR ${ENTITIES})
    cmake_path(GET ENTITYDIR FILENAME FOLDER_NAME)
    if(IS_DIRECTORY "${ENTITYDIR}")
        add_library(${FOLDER_NAME} SHARED "${ENTITYDIR}/entitymain.cpp")
        set_target_properties(${FOLDER_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/entities/${FOLDER_NAME}"
        )
        set_target_properties(${FOLDER_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/entities/${FOLDER_NAME}"
        )
        target_link_libraries(${FOLDER_NAME} PRIVATE glm::glm CobblerCore CobblerGLAD)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()
endforeach()

file(GLOB PARTICLES LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/particles/*")
file(GLOB PARTICLES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/particles/*")
foreach(PARTICLEDIR ${PARTICLES})
    cmake_path(GET PARTICLEDIR FILENAME FOLDER_NAME)
    if(IS_DIRECTORY "${PARTICLEDIR}")
        add_library(${FOLDER_NAME} SHARED "${PARTICLEDIR}/particlemain.cpp")
        set_target_properties(${FOLDER_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/particles/${FOLDER_NAME}"
        )
        set_target_properties(${FOLDER_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/particles/${FOLDER_NAME}"
        )
        target_link_libraries(${FOLDER_NAME} PRIVATE glm::glm CobblerCore CobblerGLAD)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()
endforeach()


file(GLOB CLASSES LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/class/*")
file(GLOB CLASSES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/class/*")
foreach(CLASSDIR ${CLASSES})
    cmake_path(GET CLASSDIR FILENAME FOLDER_NAME)
    if(IS_DIRECTORY "${CLASSDIR}")
        add_library(${FOLDER_NAME} SHARED "${CLASSDIR}/classmain.cpp" "${CLASSDIR}/ui.cpp" "${CLASSDIR}/entitymain.cpp")
        target_compile_definitions(${FOLDER_NAME} PRIVATE DLLEXPORT=0 UIEXPORT=0 CLASS_API=1)
        set_target_properties(${FOLDER_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/class/${FOLDER_NAME}"
        )
        set_target_properties(${FOLDER_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$CACHE{GAME}/class/${FOLDER_NAME}"
        )
        target_link_libraries(${FOLDER_NAME} PRIVATE glm::glm CobblerCore CobblerGameUI Freetype::Freetype CobblerGLAD)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/class/${FOLDER_NAME}/include)
        target_include_directories(${FOLDER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/$CACHE{GAME}/UI/include)
    endif()
endforeach()
