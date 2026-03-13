cmake_minimum_required(VERSION 3.31.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(raytracer_core
	include/Image/RGBColor.cpp
	include/Image/Image.cpp
   include/Image/Background.cpp
   include/core/ParamSet.cpp
	include/Utils/NetpbmUtils.cpp
	include/Utils/ImageUtils.cpp
	include/Utils/RunningOptions.cpp
   external/tinyxml2/tinyxml2.cpp
)
target_include_directories(raytracer_core
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(raytracer src/main.cpp)
target_link_libraries(raytracer PRIVATE raytracer_core)

find_package(Stb REQUIRED)
target_include_directories(raytracer PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(raytracer_core PRIVATE ${Stb_INCLUDE_DIR})

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(raytracer PRIVATE CLI11::CLI11)
target_link_libraries(raytracer_core PRIVATE CLI11::CLI11)

if(BUILD_TESTING)
	find_package(Catch2 3 CONFIG REQUIRED)
	add_subdirectory(tests)
endif()
