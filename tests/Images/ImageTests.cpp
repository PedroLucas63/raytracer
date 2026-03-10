#include <cstdint>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "Images/Image.hpp"

namespace {
   void requirePixel(
      const raytracer::RGBPixel& pixel,
      uint8_t red,
      uint8_t green,
      uint8_t blue
   ) {
      REQUIRE(static_cast<int>(pixel.getRed()) == static_cast<int>(red));
      REQUIRE(static_cast<int>(pixel.getGreen()) == static_cast<int>(green));
      REQUIRE(static_cast<int>(pixel.getBlue()) == static_cast<int>(blue));
   }
}

TEST_CASE("Image constructor stores dimensions channels and initializes pixels") {
   const raytracer::Image image {3, 2, 3};

   REQUIRE(image.getWidth() == 3);
   REQUIRE(image.getHeight() == 2);
   REQUIRE(image.getChannels() == 3);
   requirePixel(image.getPixel(0, 0), 0, 0, 0);
   requirePixel(image.getPixel(1, 2), 0, 0, 0);
}

TEST_CASE("Image pixel-buffer constructor copies the informed pixels") {
   raytracer::RGBPixel pixels[] {
      raytracer::RGBPixel {1, 2, 3},
      raytracer::RGBPixel {4, 5, 6},
      raytracer::RGBPixel {7, 8, 9},
      raytracer::RGBPixel {10, 11, 12}
   };

   const raytracer::Image image {pixels, 2, 2, 3};

   requirePixel(image.getPixel(0, 0), 1, 2, 3);
   requirePixel(image.getPixel(0, 1), 4, 5, 6);
   requirePixel(image.getPixel(1, 0), 7, 8, 9);
   requirePixel(image.getPixel(1, 1), 10, 11, 12);

   pixels[0] = raytracer::RGBPixel {99, 99, 99};
   requirePixel(image.getPixel(0, 0), 1, 2, 3);
}

TEST_CASE("Image constructor rejects invalid dimensions and channel counts") {
   REQUIRE_THROWS_AS(raytracer::Image(0, 2, 3), std::invalid_argument);
   REQUIRE_THROWS_AS(raytracer::Image(2, 0, 3), std::invalid_argument);
   REQUIRE_THROWS_AS(raytracer::Image(2, 2, 2), std::invalid_argument);
}

TEST_CASE("Image setPixel updates pixels and operator() returns the stored value") {
   raytracer::Image image {3, 2, 3};

   image.setPixel(raytracer::RGBPixel {12, 34, 56}, 1, 2);

   requirePixel(image.getPixel(1, 2), 12, 34, 56);
   requirePixel(image(1, 2), 12, 34, 56);
}

TEST_CASE("Image setPixel accepts valid edge coordinates and rejects out-of-bounds indices") {
   raytracer::Image image {3, 2, 3};

   REQUIRE_NOTHROW(image.setPixel(raytracer::RGBPixel {1, 2, 3}, 1, 2));
   REQUIRE_THROWS_AS(image.setPixel(raytracer::RGBPixel {1, 2, 3}, 2, 0), std::out_of_range);
   REQUIRE_THROWS_AS(image.setPixel(raytracer::RGBPixel {1, 2, 3}, 0, 3), std::out_of_range);
}

TEST_CASE("Image toGrayScale converts every image pixel independently") {
   raytracer::Image image {2, 1, 3};
   image.setPixel(raytracer::RGBPixel {255, 0, 0}, 0, 0);
   image.setPixel(raytracer::RGBPixel {0, 255, 0}, 0, 1);

   const raytracer::Image grayImage = image.toGrayScale();

   REQUIRE(grayImage.getWidth() == 2);
   REQUIRE(grayImage.getHeight() == 1);
   requirePixel(grayImage.getPixel(0, 0), 76, 76, 76);
   requirePixel(grayImage.getPixel(0, 1), 150, 150, 150);
   requirePixel(image.getPixel(0, 0), 255, 0, 0);
   requirePixel(image.getPixel(0, 1), 0, 255, 0);
}
