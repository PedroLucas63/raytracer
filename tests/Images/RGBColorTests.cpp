#include <cstdint>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "Images/RGBColor.hpp"

namespace {
   void requirePixel(
      const raytracer::RGBColor& pixel,
      uint8_t red,
      uint8_t green,
      uint8_t blue
   ) {
      REQUIRE(static_cast<int>(pixel.getRed()) == static_cast<int>(red));
      REQUIRE(static_cast<int>(pixel.getGreen()) == static_cast<int>(green));
      REQUIRE(static_cast<int>(pixel.getBlue()) == static_cast<int>(blue));
   }
}

TEST_CASE("RGBColor default constructor initializes a black pixel") {
   const raytracer::RGBColor pixel {};

   requirePixel(pixel, 0, 0, 0);
}

TEST_CASE("RGBColor constructor stores the informed channels") {
   const raytracer::RGBColor pixel {12, 34, 56};

   requirePixel(pixel, 12, 34, 56);
}

TEST_CASE("RGBColor setters update channels independently") {
   raytracer::RGBColor pixel {};

   pixel.setRed(10);
   pixel.setGreen(20);
   pixel.setBlue(30);

   requirePixel(pixel, 10, 20, 30);
}

TEST_CASE("RGBColor channel operator returns the selected channel") {
   const raytracer::RGBColor pixel {10, 20, 30};

   REQUIRE(static_cast<int>(pixel[raytracer::RGBChannel::RED]) == 10);
   REQUIRE(static_cast<int>(pixel[raytracer::RGBChannel::GREEN]) == 20);
   REQUIRE(static_cast<int>(pixel[raytracer::RGBChannel::BLUE]) == 30);
}

TEST_CASE("RGBColor channel operator rejects invalid channels") {
   const raytracer::RGBColor pixel {10, 20, 30};

   REQUIRE_THROWS_AS(
      pixel[static_cast<raytracer::RGBChannel>(99)],
      std::out_of_range
   );
}

TEST_CASE("RGBColor predefined constants expose the expected colors") {
   requirePixel(raytracer::PIXEL_BLACK, 0, 0, 0);
   requirePixel(raytracer::PIXEL_WHITE, 255, 255, 255);
   requirePixel(raytracer::PIXEL_RED, 255, 0, 0);
   requirePixel(raytracer::PIXEL_GREEN, 0, 255, 0);
   requirePixel(raytracer::PIXEL_BLUE, 0, 0, 255);
   requirePixel(raytracer::PIXEL_YELLOW, 255, 255, 0);
   requirePixel(raytracer::PIXEL_MAGENTA, 255, 0, 255);
   requirePixel(raytracer::PIXEL_CYAN, 0, 255, 255);
}

TEST_CASE("RGBColor addition sums channels and saturates at 255") {
   const raytracer::RGBColor left {100, 120, 140};
   const raytracer::RGBColor right {10, 20, 30};
   const raytracer::RGBColor saturatedLeft {250, 240, 230};
   const raytracer::RGBColor saturatedRight {10, 20, 30};

   requirePixel(left + right, 110, 140, 170);
   requirePixel(saturatedLeft + saturatedRight, 255, 255, 255);
   requirePixel(left, 100, 120, 140);
}

TEST_CASE("RGBColor compound addition mutates the pixel and returns itself") {
   raytracer::RGBColor pixel {250, 240, 230};

   auto& result = (pixel += raytracer::RGBColor {10, 20, 30});

   REQUIRE(&result == &pixel);
   requirePixel(pixel, 255, 255, 255);
}

TEST_CASE("RGBColor subtraction subtracts channels and saturates at zero") {
   const raytracer::RGBColor left {100, 120, 140};
   const raytracer::RGBColor right {10, 20, 30};
   const raytracer::RGBColor saturatedLeft {5, 10, 15};
   const raytracer::RGBColor saturatedRight {10, 20, 30};

   requirePixel(left - right, 90, 100, 110);
   requirePixel(saturatedLeft - saturatedRight, 0, 0, 0);
   requirePixel(left, 100, 120, 140);
}

TEST_CASE("RGBColor compound subtraction mutates the pixel and returns itself") {
   raytracer::RGBColor pixel {5, 10, 15};

   auto& result = (pixel -= raytracer::RGBColor {10, 20, 30});

   REQUIRE(&result == &pixel);
   requirePixel(pixel, 0, 0, 0);
}

TEST_CASE("RGBColor multiplication scales channels with rounding and saturation") {
   const raytracer::RGBColor pixel {5, 15, 25};
   const raytracer::RGBColor saturatedPixel {200, 220, 240};

   requirePixel(pixel * 1.2, 6, 18, 30);
   requirePixel(pixel * 0.5, 3, 8, 13);
   requirePixel(saturatedPixel * 2.0, 255, 255, 255);
   requirePixel(pixel, 5, 15, 25);
}

TEST_CASE("RGBColor compound multiplication mutates the pixel and returns itself") {
   raytracer::RGBColor pixel {10, 20, 30};

   auto& result = (pixel *= 0.5);

   REQUIRE(&result == &pixel);
   requirePixel(pixel, 5, 10, 15);
}

TEST_CASE("RGBColor multiplication rejects negative scales") {
   const raytracer::RGBColor pixel {10, 20, 30};

   REQUIRE_THROWS_AS(pixel * -0.5, std::invalid_argument);

   auto mutablePixel = pixel;
   REQUIRE_THROWS_AS(mutablePixel *= -0.5, std::invalid_argument);
}

TEST_CASE("RGBColor division scales channels down with rounding") {
   const raytracer::RGBColor pixel {10, 21, 31};

   requirePixel(pixel / 2.0, 5, 11, 16);
   requirePixel(pixel, 10, 21, 31);
}

TEST_CASE("RGBColor compound division mutates the pixel and returns itself") {
   raytracer::RGBColor pixel {10, 21, 31};

   auto& result = (pixel /= 2.0);

   REQUIRE(&result == &pixel);
   requirePixel(pixel, 5, 11, 16);
}

TEST_CASE("RGBColor division rejects non-positive scales") {
   const raytracer::RGBColor pixel {10, 20, 30};

   REQUIRE_THROWS_AS(pixel / 0.0, std::invalid_argument);
   REQUIRE_THROWS_AS(pixel / -1.0, std::invalid_argument);

   auto mutablePixel = pixel;
   REQUIRE_THROWS_AS(mutablePixel /= 0.0, std::invalid_argument);
   REQUIRE_THROWS_AS(mutablePixel /= -1.0, std::invalid_argument);
}

TEST_CASE("RGBColor toGrayScale converts a pixel using the luminance formula") {
   const raytracer::RGBColor pixel {10, 20, 30};

   const raytracer::RGBColor grayPixel = pixel.toGrayScale();

   requirePixel(grayPixel, 18, 18, 18);
   requirePixel(pixel, 10, 20, 30);
}

TEST_CASE("RGBColor toGrayScale keeps grayscale pixels unchanged") {
   const raytracer::RGBColor pixel {77, 77, 77};

   const raytracer::RGBColor grayPixel = pixel.toGrayScale();

   requirePixel(grayPixel, 77, 77, 77);
}

TEST_CASE("RGBColor toGrayScale maps extreme colors to their grayscale equivalent") {
   requirePixel(raytracer::PIXEL_BLACK.toGrayScale(), 0, 0, 0);
   requirePixel(raytracer::PIXEL_WHITE.toGrayScale(), 255, 255, 255);
   requirePixel(raytracer::PIXEL_RED.toGrayScale(), 76, 76, 76);
}