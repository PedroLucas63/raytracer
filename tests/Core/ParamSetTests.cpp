#include <stdexcept>
#include <string>
#include <sstream>    

#include <catch2/catch_test_macros.hpp>

#include "Core/ParamSet.hpp"
#include "Image/RGBColor.hpp"

namespace {
   /**
    * Simulates parsing "R G B" strings from XML attribute values,
    * e.g. bl="0 0 51" → RGBColor{0, 0, 51}
    */
   raytracer::RGBColor parseColor(const std::string& s) {
      int r, g, b;
      std::istringstream ss(s);
      ss >> r >> g >> b;
      return raytracer::RGBColor{
         static_cast<uint8_t>(r),
         static_cast<uint8_t>(g),
         static_cast<uint8_t>(b)
      };
   }
 
   void requireColor(
      const raytracer::RGBColor& color,
      uint8_t r, uint8_t g, uint8_t b
   ) {
      REQUIRE(static_cast<int>(color.getRed())   == static_cast<int>(r));
      REQUIRE(static_cast<int>(color.getGreen()) == static_cast<int>(g));
      REQUIRE(static_cast<int>(color.getBlue())  == static_cast<int>(b));
   }
}

// ── background type="4_colors" ────────────────────────────────────────────────
 
TEST_CASE("ParamSet stores and resolves 4-color background from XML attributes") {
   // Simulates parsing:
   // <background type="4_colors" bl="0 0 51" tl="0 255 51" tr="255 255 51" br="255 0 51" />
   raytracer::ParamSet ps;
 
   ps.add<std::string>("type", "4_colors");
   ps.add<raytracer::RGBColor>("bl", parseColor("0 0 51"));
   ps.add<raytracer::RGBColor>("tl", parseColor("0 255 51"));
   ps.add<raytracer::RGBColor>("tr", parseColor("255 255 51"));
   ps.add<raytracer::RGBColor>("br", parseColor("255 0 51"));
 
   REQUIRE(ps.retrieve<std::string>("type") == "4_colors");
 
   requireColor(ps.retrieve<raytracer::RGBColor>("bl"),   0,   0,  51);
   requireColor(ps.retrieve<raytracer::RGBColor>("tl"),   0, 255,  51);
   requireColor(ps.retrieve<raytracer::RGBColor>("tr"), 255, 255,  51);
   requireColor(ps.retrieve<raytracer::RGBColor>("br"), 255,   0,  51);
}
 
TEST_CASE("ParamSet resolves correct corner after overwriting a color") {
   raytracer::ParamSet ps;
 
   ps.add<raytracer::RGBColor>("tl", parseColor("0 0 0"));
   ps.add<raytracer::RGBColor>("tl", parseColor("10 20 30")); // overwrite
 
   requireColor(ps.retrieve<raytracer::RGBColor>("tl"), 10, 20, 30);
}
 
TEST_CASE("ParamSet throws when resolving a color key as wrong type") {
   raytracer::ParamSet ps;
 
   ps.add<raytracer::RGBColor>("bl", parseColor("0 0 51"));
 
   REQUIRE_THROWS_AS(ps.retrieve<std::string>("bl"), std::bad_cast);
}
 
TEST_CASE("ParamSet correctly distinguishes all four corner keys") {
   raytracer::ParamSet ps;
 
   ps.add<raytracer::RGBColor>("tl", parseColor("10 10 10"));
   ps.add<raytracer::RGBColor>("tr", parseColor("20 20 20"));
   ps.add<raytracer::RGBColor>("bl", parseColor("30 30 30"));
   ps.add<raytracer::RGBColor>("br", parseColor("40 40 40"));
 
   requireColor(ps.retrieve<raytracer::RGBColor>("tl"), 10, 10, 10);
   requireColor(ps.retrieve<raytracer::RGBColor>("tr"), 20, 20, 20);
   requireColor(ps.retrieve<raytracer::RGBColor>("bl"), 30, 30, 30);
   requireColor(ps.retrieve<raytracer::RGBColor>("br"), 40, 40, 40);
}
 
// ── background type="solid" ───────────────────────────────────────────────────
 
TEST_CASE("ParamSet stores and resolves solid background") {
   // <background type="solid" color="100 150 200" />
   raytracer::ParamSet ps;
 
   ps.add<std::string>("type",           "solid");
   ps.add<raytracer::RGBColor>("color",  parseColor("100 150 200"));
 
   REQUIRE(ps.retrieve<std::string>("type") == "solid");
   requireColor(ps.retrieve<raytracer::RGBColor>("color"), 100, 150, 200);
}
 
// ── background type="gradient" ────────────────────────────────────────────────
 
TEST_CASE("ParamSet stores and resolves horizontal gradient background") {
   // <background type="horizontal_gradient" from="0 0 0" to="255 255 255" />
   raytracer::ParamSet ps;
 
   ps.add<std::string>("type",          "horizontal_gradient");
   ps.add<raytracer::RGBColor>("from",  parseColor("0 0 0"));
   ps.add<raytracer::RGBColor>("to",    parseColor("255 255 255"));
 
   REQUIRE(ps.retrieve<std::string>("type") == "horizontal_gradient");
   requireColor(ps.retrieve<raytracer::RGBColor>("from"),   0,   0,   0);
   requireColor(ps.retrieve<raytracer::RGBColor>("to"),   255, 255, 255);
}

// ── add / resolve ─────────────────────────────────────────────────────────────

TEST_CASE("ParamSet resolve returns the value that was added") {
   raytracer::ParamSet ps;

   ps.add<int>("width", 800);

   REQUIRE(ps.retrieve<int>("width") == 800);
}

TEST_CASE("ParamSet resolve works for multiple types") {
   raytracer::ParamSet ps;

   ps.add<int>("samples",          16);
   ps.add<float>("fov",            45.0f);
   ps.add<std::string>("name",     "scene");

   REQUIRE(ps.retrieve<int>("samples")         == 16);
   REQUIRE(ps.retrieve<float>("fov")           == 45.0f);
   REQUIRE(ps.retrieve<std::string>("name")    == "scene");
}

TEST_CASE("ParamSet add overwrites a previously stored value") {
   raytracer::ParamSet ps;

   ps.add<int>("depth", 4);
   ps.add<int>("depth", 8);

   REQUIRE(ps.retrieve<int>("depth") == 8);
}

// ── resolve — exceções ────────────────────────────────────────────────────────

TEST_CASE("ParamSet resolve throws out_of_range for missing key") {
   raytracer::ParamSet ps;

   REQUIRE_THROWS_AS(ps.retrieve<int>("missing"), std::out_of_range);
}

TEST_CASE("ParamSet resolve throws bad_cast when type does not match") {
   raytracer::ParamSet ps;

   ps.add<int>("value", 42);

   REQUIRE_THROWS_AS(ps.retrieve<float>("value"), std::bad_cast);
}

// ── has ───────────────────────────────────────────────────────────────────────

TEST_CASE("ParamSet has returns false on empty set") {
   raytracer::ParamSet ps;

   REQUIRE_FALSE(ps.has("anything"));
}

TEST_CASE("ParamSet has returns true after add") {
   raytracer::ParamSet ps;

   ps.add<int>("x", 1);

   REQUIRE(ps.has("x"));
}

TEST_CASE("ParamSet has returns false after remove") {
   raytracer::ParamSet ps;

   ps.add<int>("x", 1);
   ps.remove("x");

   REQUIRE_FALSE(ps.has("x"));
}

// ── remove ────────────────────────────────────────────────────────────────────

TEST_CASE("ParamSet remove makes key unreachable via resolve") {
   raytracer::ParamSet ps;

   ps.add<int>("temp", 99);
   ps.remove("temp");

   REQUIRE_THROWS_AS(ps.retrieve<int>("temp"), std::out_of_range);
}

TEST_CASE("ParamSet remove on non-existent key does not throw") {
   raytracer::ParamSet ps;

   REQUIRE_NOTHROW(ps.remove("ghost"));
}

// ── size ──────────────────────────────────────────────────────────────────────

TEST_CASE("ParamSet size reflects the number of stored entries") {
   raytracer::ParamSet ps;

   REQUIRE(ps.size() == 0);

   ps.add<int>("a", 1);
   REQUIRE(ps.size() == 1);

   ps.add<int>("b", 2);
   REQUIRE(ps.size() == 2);

   ps.add<int>("a", 3); // overwrite — size must stay 2
   REQUIRE(ps.size() == 2);
}

// ── isEmpty ───────────────────────────────────────────────────────────────────

TEST_CASE("ParamSet isEmpty returns true only when empty") {
   raytracer::ParamSet ps;

   REQUIRE(ps.isEmpty());

   ps.add<int>("k", 0);
   REQUIRE_FALSE(ps.isEmpty());
}

// ── clear ─────────────────────────────────────────────────────────────────────

TEST_CASE("ParamSet clear removes all entries") {
   raytracer::ParamSet ps;

   ps.add<int>("a", 1);
   ps.add<int>("b", 2);
   ps.clear();

   REQUIRE(ps.isEmpty());
   REQUIRE(ps.size() == 0);
   REQUIRE_FALSE(ps.has("a"));
   REQUIRE_FALSE(ps.has("b"));
}

TEST_CASE("ParamSet clear on empty set does not throw") {
   raytracer::ParamSet ps;

   REQUIRE_NOTHROW(ps.clear());
}