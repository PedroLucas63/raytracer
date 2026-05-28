#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Core/ParamSet.hpp"
#include "Image/RGBColor.hpp"
#include "Utils/ConvertFunction.hpp"

TEST_CASE("convert<T> stores a valid integer value") {
   raytracer::ParamSet ps;

   REQUIRE(raytracer::convert<int>("samples", "42", &ps));
   REQUIRE(ps.retrieve<int>("samples") == 42);
}

TEST_CASE("convert<T> stores a valid floating point value") {
   raytracer::ParamSet ps;

   REQUIRE(raytracer::convert<float>("gamma", "2.2", &ps));
   REQUIRE(ps.retrieve<float>("gamma") == Catch::Approx(2.2f));
}

TEST_CASE("convert<T> parses boolean values with boolalpha") {
   raytracer::ParamSet ps;

   REQUIRE(raytracer::convert<bool>("quick", "true", &ps));
   REQUIRE(ps.retrieve<bool>("quick"));
}

TEST_CASE("convert<T> throws for empty content") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      raytracer::convert<int>("samples", "", &ps),
      std::invalid_argument
   );
}

TEST_CASE("convert<T> throws when passing text for an integer") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      convert<int>("samples", "not-a-number", &ps),
      std::invalid_argument
   );
}

TEST_CASE("convert<T> throws when there are mixed token types") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      convert<int>("samples", "10x", &ps),
      std::invalid_argument
   );
}

TEST_CASE("convert<T,N> stores a valid fixed-size vector") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<int>, 3>("rgb", "10 20 30", &ps));

   const auto rgb = ps.retrieve<std::vector<int>>("rgb");
   REQUIRE(rgb.size() == 3);
   REQUIRE(rgb[0] == 10);
   REQUIRE(rgb[1] == 20);
   REQUIRE(rgb[2] == 30);
}

TEST_CASE("convert<T,N> stores a valid fixed-size boolean vector") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<bool>, 3>("flags", "true false true", &ps));

   const auto flags = ps.retrieve<std::vector<bool>>("flags");
   REQUIRE(flags.size() == 3);
   REQUIRE(flags[0]);
   REQUIRE_FALSE(flags[1]);
   REQUIRE(flags[2]);
}

TEST_CASE("convert<T,N> throws when fewer members than expected are provided") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      (convert<std::vector<int>, 3>("rgb", "10 20", &ps)),
      std::invalid_argument
   );
}

TEST_CASE("convert<T,N> throws when more members than expected are provided") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      (convert<std::vector<int>, 3>("rgb", "10 20 30 40", &ps)),
      std::invalid_argument
   );
}

TEST_CASE("convert<std::vector<T>> stores a valid dynamic integer vector") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<int>>("samples", "1 2 3 4 5", &ps));

   const auto samples = ps.retrieve<std::vector<int>>("samples");
   REQUIRE(samples.size() == 5);
   REQUIRE(samples[0] == 1);
   REQUIRE(samples[4] == 5);
}

TEST_CASE("convert<std::vector<RGBColor>> parses compact RGB triplets") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<raytracer::RGBColor>>("palette", "255 255 255 0 0 0 255 0 0", &ps));

   const auto palette = ps.retrieve<std::vector<raytracer::RGBColor>>("palette");
   REQUIRE(palette.size() == 3);
   REQUIRE(palette[0].getRed() == 255);
   REQUIRE(palette[0].getGreen() == 255);
   REQUIRE(palette[0].getBlue() == 255);
   REQUIRE(palette[1].getRed() == 0);
   REQUIRE(palette[1].getGreen() == 0);
   REQUIRE(palette[1].getBlue() == 0);
   REQUIRE(palette[2].getRed() == 255);
   REQUIRE(palette[2].getGreen() == 0);
   REQUIRE(palette[2].getBlue() == 0);
}

TEST_CASE("convert<std::vector<RGBColor>> throws when member count is not multiple of 3") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      convert<std::vector<raytracer::RGBColor>>("palette", "255 255 255 0", &ps),
      std::invalid_argument
   );
}
