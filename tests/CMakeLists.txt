add_executable(raytracer_tests
   Image/RGBColorTests.cpp
   Image/ImageTests.cpp
   Image/BackgroundTests.cpp
   Utils/ImageUtilsTests.cpp
   Utils/NetpbmUtilsTests.cpp
   Core/ParamSetTests.cpp
)

target_link_libraries(raytracer_tests
   PRIVATE
      Catch2::Catch2WithMain
      raytracer_core
)

include(Catch)
catch_discover_tests(raytracer_tests)