cmake_minimum_required(VERSION 3.20)
project(wallbounce)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(raylib CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include_directories("imgui" "rlImGui")

set(RLIMGUI_SOURCES "rlImGui/rlImGui.cpp")
add_library(rlimgui STATIC ${RLIMGUI_SOURCES})

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS "imgui/*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})

if(EMSCRIPTEN)
	add_compile_options(-DPLATFORM_WEB)
	add_link_options(-sUSE_GLFW=3 -sASYNCIFY -sASSERTIONS=1 -sWASM=1 --preload-file=../assets -sFORCE_FILESYSTEM)
endif()

add_executable(wallbounce ${SOURCES})
target_link_libraries(wallbounce raylib m rlimgui imgui)
target_include_directories(wallbounce PRIVATE "imgui" "rlImGui")
target_precompile_headers(wallbounce PRIVATE src/pch.hh)
