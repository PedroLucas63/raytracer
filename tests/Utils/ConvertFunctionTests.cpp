#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Core/ParamSet.hpp"
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

TEST_CASE("convert<T,K,N> stores a valid fixed-size vector") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<int>, int, 3>("rgb", "10 20 30", &ps));

   const auto rgb = ps.retrieve<std::vector<int>>("rgb");
   REQUIRE(rgb.size() == 3);
   REQUIRE(rgb[0] == 10);
   REQUIRE(rgb[1] == 20);
   REQUIRE(rgb[2] == 30);
}

TEST_CASE("convert<T,K,N> stores a valid fixed-size boolean vector") {
   raytracer::ParamSet ps;

   REQUIRE(convert<std::vector<bool>, bool, 3>("flags", "1 0 1", &ps));

   const auto flags = ps.retrieve<std::vector<bool>>("flags");
   REQUIRE(flags.size() == 3);
   REQUIRE(flags[0]);
   REQUIRE_FALSE(flags[1]);
   REQUIRE(flags[2]);
}

TEST_CASE("convert<T,K,N> throws when fewer members than expected are provided") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      (convert<std::vector<int>, int, 3>("rgb", "10 20", &ps)),
      std::invalid_argument
   );
}

TEST_CASE("convert<T,K,N> throws when more members than expected are provided") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(
      (convert<std::vector<int>, int, 3>("rgb", "10 20 30 40", &ps)),
      std::invalid_argument
   );
}
