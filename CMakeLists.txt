cmake_minimum_required(VERSION 3.31.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(raytracer src/main.cpp)

find_package(Stb REQUIRED)
target_include_directories(raytracer PRIVATE ${Stb_INCLUDE_DIR})
