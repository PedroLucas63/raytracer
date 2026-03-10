cmake_minimum_required(VERSION 3.31.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(raytracer_core
	include/Images/RGBPixel.cpp
)
target_include_directories(raytracer_core
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(raytracer src/main.cpp)
target_link_libraries(raytracer PRIVATE raytracer_core)

find_package(Stb REQUIRED)
target_include_directories(raytracer PRIVATE ${Stb_INCLUDE_DIR})

if(BUILD_TESTING)
	find_package(Catch2 3 CONFIG REQUIRED)
	add_subdirectory(tests)
endif()
