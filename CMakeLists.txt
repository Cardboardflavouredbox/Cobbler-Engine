cmake_minimum_required(VERSION 3.10)
project(FPSthing)

set(CMAKE_CXX_STANDARD 17)

# SDL2 패키지 찾기
find_package(SDL2 REQUIRED)

add_executable(FPSthing src/main.cpp)

# SDL2 라이브러리 연결
target_link_libraries(FPSthing PRIVATE SDL2::SDL2)
