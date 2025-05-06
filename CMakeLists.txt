cmake_minimum_required(VERSION 3.20)
project(wallbounce)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(raylib CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

add_compile_options(-DPLATFORM_WEB)
add_link_options(-sUSE_GLFW=3 -sASYNCIFY)

add_executable(wallbounce ${SOURCES})
target_link_libraries(wallbounce raylib m)
target_precompile_headers(wallbounce PRIVATE src/pch.hh)
