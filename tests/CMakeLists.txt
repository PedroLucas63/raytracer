add_executable(raytracer_tests
   Images/RGBPixelTests.cpp
)

target_link_libraries(raytracer_tests
   PRIVATE
      Catch2::Catch2WithMain
      raytracer_core
)

include(Catch)
catch_discover_tests(raytracer_tests)