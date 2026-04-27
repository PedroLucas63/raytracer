cmake_minimum_required(VERSION 3.31.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(raytracer_core
	include/Image/RGBColor.cpp
	include/Image/Image.cpp
	include/Scene/Background/BackgroundImage.cpp
	include/Scene/Background/BackgroundFactory.cpp
	include/Scene/Background/BackgroundColor.cpp
	include/Scene/Camera.cpp
	include/Scene/Film.cpp
	include/Scene/Scene.cpp
	include/Objects/Materials/Material.cpp
	include/Objects/Materials/GridMaterial.cpp
	include/Objects/Materials/FlatMaterial.cpp
	include/Objects/Materials/BlinnMaterial.cpp
	include/Objects/Materials/MaterialFactory.cpp
	include/Objects/PrimitiveFactory.cpp
	include/Objects/GeometricPrimitive.cpp
	include/Objects/Shapes/Sphere.cpp
	include/Objects/Shapes/Plan.cpp
	include/Objects/Aggregate/AggregatePrimitive.cpp
	include/Objects/Aggregate/PrimitiveList.cpp
	include/Integrator/Integrator.cpp
	include/Integrator/RayCastIntegrator.cpp
	include/Integrator/DepthMapIntegrator.cpp
	include/Integrator/NormalMapIntegrator.cpp
	include/Integrator/BlinnPhongIntegrator.cpp
	include/Integrator/IntegratorFactory.cpp
	include/Core/ParamSet.cpp
	include/Math/Vector3.cpp
	include/Math/Point3.cpp
	include/Math/Bounds3.cpp
	include/Math/Point2.cpp
	include/Utils/ImageUtils.cpp
	include/Utils/RunningOptions.cpp
	include/Utils/ConvertFunction.cpp
	include/Utils/ProgressBar.cpp
	include/Parser/ParserScene.cpp
	include/Api/Api.cpp
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

find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(raytracer PRIVATE tinyxml2::tinyxml2)
target_link_libraries(raytracer_core PRIVATE tinyxml2::tinyxml2)

if(BUILD_TESTING)
	find_package(Catch2 3 CONFIG REQUIRED)
	add_subdirectory(tests)
endif()
