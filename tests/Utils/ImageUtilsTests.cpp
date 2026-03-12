#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "Images/Image.hpp"
#include "Utils/ImageUtils.hpp"

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

   void requirePixelNear(
      const raytracer::RGBColor& pixel,
      uint8_t red,
      uint8_t green,
      uint8_t blue,
      uint8_t tolerance
   ) {
      REQUIRE(std::abs(static_cast<int>(pixel.getRed()) - static_cast<int>(red)) <= tolerance);
      REQUIRE(std::abs(static_cast<int>(pixel.getGreen()) - static_cast<int>(green)) <= tolerance);
      REQUIRE(std::abs(static_cast<int>(pixel.getBlue()) - static_cast<int>(blue)) <= tolerance);
   }

   class TemporaryDirectory {
      public:
         TemporaryDirectory() {
            const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
            _path = std::filesystem::temp_directory_path() /
               ("raytracer_image_utils_tests_" + std::to_string(uniqueId));
            std::filesystem::create_directories(_path);
         }

         ~TemporaryDirectory() {
            std::error_code errorCode;
            std::filesystem::remove_all(_path, errorCode);
         }

         const std::filesystem::path& path() const {
            return _path;
         }

      private:
         std::filesystem::path _path;
   };

   void populateRgbImage(raytracer::Image& image) {
      image.setPixel(raytracer::RGBColor {255, 0, 0}, 0, 0);
      image.setPixel(raytracer::RGBColor {0, 255, 0}, 0, 1);
      image.setPixel(raytracer::RGBColor {0, 0, 255}, 1, 0);
      image.setPixel(raytracer::RGBColor {255, 255, 255}, 1, 1);
   }

   void requirePopulatedRgbImage(const raytracer::Image& image) {
      REQUIRE(image.getWidth() == 2);
      REQUIRE(image.getHeight() == 2);
      REQUIRE(image.getChannels() == 3);

      requirePixel(image.getPixel(0, 0), 255, 0, 0);
      requirePixel(image.getPixel(0, 1), 0, 255, 0);
      requirePixel(image.getPixel(1, 0), 0, 0, 255);
      requirePixel(image.getPixel(1, 1), 255, 255, 255);
   }
}

TEST_CASE("ImageUtils saveImage and loadImage round-trip lossless formats") {
   TemporaryDirectory temporaryDirectory;
   raytracer::Image image {2, 2, 3};
   populateRgbImage(image);

   auto verifyRoundTrip = [&](const std::filesystem::path& outputPath, bool useDefaultType, raytracer::ImageType type) {
      if (useDefaultType) {
         raytracer::saveImage(image, outputPath.string());
      } else {
         raytracer::saveImage(image, outputPath.string(), type);
      }

      REQUIRE(std::filesystem::exists(outputPath));
      REQUIRE(std::filesystem::file_size(outputPath) > 0);

      const raytracer::Image loaded = raytracer::loadImage(outputPath.string());
      requirePopulatedRgbImage(loaded);
   };

   SECTION("PNG default overload") {
      verifyRoundTrip(temporaryDirectory.path() / "sample.png", true, raytracer::PNG);
   }

   SECTION("BMP") {
      verifyRoundTrip(temporaryDirectory.path() / "sample.bmp", false, raytracer::BMP);
   }

   SECTION("TGA") {
      verifyRoundTrip(temporaryDirectory.path() / "sample.tga", false, raytracer::TGA);
   }
}

TEST_CASE("ImageUtils saveImage and loadImage preserve grayscale images") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "grayscale.png";

   raytracer::Image image {2, 1, 1};
   image.setPixel(raytracer::RGBColor {30, 30, 30}, 0, 0);
   image.setPixel(raytracer::RGBColor {220, 220, 220}, 0, 1);

   raytracer::saveImage(image, outputPath.string());

   const raytracer::Image loaded = raytracer::loadImage(outputPath.string());

   REQUIRE(loaded.getWidth() == 2);
   REQUIRE(loaded.getHeight() == 1);
   REQUIRE(loaded.getChannels() == 1);
   requirePixel(loaded.getPixel(0, 0), 30, 30, 30);
   requirePixel(loaded.getPixel(0, 1), 220, 220, 220);
}

TEST_CASE("ImageUtils saveImage writes JPEG files that can be loaded back") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "sample.jpg";

   raytracer::Image image {8, 8, 3};
   for (uint16_t row = 0; row < image.getHeight(); row++) {
      for (uint16_t col = 0; col < image.getWidth(); col++) {
         image.setPixel(raytracer::RGBColor {40, 120, 200}, row, col);
      }
   }

   raytracer::saveImage(image, outputPath.string(), raytracer::JPG);

   REQUIRE(std::filesystem::exists(outputPath));
   REQUIRE(std::filesystem::file_size(outputPath) > 0);

   const raytracer::Image loaded = raytracer::loadImage(outputPath.string());

   REQUIRE(loaded.getWidth() == 8);
   REQUIRE(loaded.getHeight() == 8);
   REQUIRE(loaded.getChannels() == 3);
   requirePixelNear(loaded.getPixel(0, 0), 40, 120, 200, 10);
   requirePixelNear(loaded.getPixel(7, 7), 40, 120, 200, 10);
}

TEST_CASE("ImageUtils loadImage throws when the file cannot be loaded") {
   TemporaryDirectory temporaryDirectory;
   const auto missingPath = temporaryDirectory.path() / "missing.png";

   REQUIRE_THROWS_AS(raytracer::loadImage(missingPath.string()), std::runtime_error);
}

TEST_CASE("ImageUtils saveImage rejects unsupported formats") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "unsupported.img";

   raytracer::Image image {1, 1, 3};
   image.setPixel(raytracer::RGBColor {1, 2, 3}, 0, 0);

   REQUIRE_THROWS_AS(
      raytracer::saveImage(image, outputPath.string(), static_cast<raytracer::ImageType>(999)),
      std::invalid_argument
   );
}