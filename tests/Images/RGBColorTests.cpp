#include <cmath>
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

   void requireFloatNear(float actual, float expected, float epsilon = 0.0001f) {
      REQUIRE(std::fabs(actual - expected) <= epsilon);
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

TEST_CASE("RGBColor fromNormalized converts normalized channels into byte channels") {
   const raytracer::RGBColor pixel = raytracer::RGBColor::fromNormalized(0.0f, 0.5f, 1.0f);

   requirePixel(pixel, 0, 128, 255);
}

TEST_CASE("RGBColor fromNormalized clamps normalized channels outside the valid range") {
   const raytracer::RGBColor pixel = raytracer::RGBColor::fromNormalized(-0.1f, 0.0f, 1.5f);

   requirePixel(pixel, 0, 0, 255);
}

TEST_CASE("RGBColor normalized getters expose channels in the unit interval") {
   const raytracer::RGBColor pixel {0, 128, 255};

   requireFloatNear(pixel.getRedNormalized(), 0.0f);
   requireFloatNear(pixel.getGreenNormalized(), 128.0f / 255.0f);
   requireFloatNear(pixel.getBlueNormalized(), 1.0f);
}

TEST_CASE("RGBColor setters update channels independently") {
   raytracer::RGBColor pixel {};

   pixel.setRed(10);
   pixel.setGreen(20);
   pixel.setBlue(30);

   requirePixel(pixel, 10, 20, 30);
}

TEST_CASE("RGBColor normalized setters update channels independently") {
   raytracer::RGBColor pixel {};

   pixel.setRedNormalized(0.0f);
   pixel.setGreenNormalized(0.5f);
   pixel.setBlueNormalized(1.0f);

   requirePixel(pixel, 0, 128, 255);
}

TEST_CASE("RGBColor normalized setters clamp values outside the unit interval") {
   raytracer::RGBColor pixel {10, 20, 30};

   pixel.setRedNormalized(-0.25f);
   pixel.setGreenNormalized(1.25f);
   pixel.setBlueNormalized(0.0f);

   requirePixel(pixel, 0, 255, 0);
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

TEST_CASE("RGBColor interpolate returns the other color at t equals zero") {
   const raytracer::RGBColor source {10, 20, 30};
   const raytracer::RGBColor target {110, 120, 130};

   const raytracer::RGBColor interpolated = source.interpolate(target, 0.0);

   requirePixel(interpolated, 110, 120, 130);
}

TEST_CASE("RGBColor interpolate returns the source color at t equals one") {
   const raytracer::RGBColor source {10, 20, 30};
   const raytracer::RGBColor target {110, 120, 130};

   const raytracer::RGBColor interpolated = source.interpolate(target, 1.0);

   requirePixel(interpolated, 10, 20, 30);
}

TEST_CASE("RGBColor interpolate weighs the source by t with rounding") {
   const raytracer::RGBColor source {0, 0, 0};
   const raytracer::RGBColor target {255, 255, 255};

   requirePixel(source.interpolate(target, 0.25), 191, 191, 191);
   requirePixel(source.interpolate(target, 0.50), 128, 128, 128);
   requirePixel(source.interpolate(target, 0.75), 64, 64, 64);
}

TEST_CASE("RGBColor interpolate does not mutate either color") {
   const raytracer::RGBColor source {15, 25, 35};
   const raytracer::RGBColor target {115, 125, 135};

   const raytracer::RGBColor interpolated = source.interpolate(target, 0.5);

   requirePixel(interpolated, 65, 75, 85);
   requirePixel(source, 15, 25, 35);
   requirePixel(target, 115, 125, 135);
}

TEST_CASE("RGBColor interpolate rejects interpolation factors outside the unit interval") {
   const raytracer::RGBColor source {10, 20, 30};
   const raytracer::RGBColor target {110, 120, 130};

   REQUIRE_THROWS_AS(source.interpolate(target, -0.1), std::invalid_argument);
   REQUIRE_THROWS_AS(source.interpolate(target, 1.1), std::invalid_argument);
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