#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "Image/Image.hpp"
#include "Utils/NetpbmUtils.hpp"

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

   class TemporaryDirectory {
      public:
         TemporaryDirectory() {
            const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
            _path = std::filesystem::temp_directory_path() /
               ("raytracer_netpbm_tests_" + std::to_string(uniqueId));
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

   std::string readTextFile(const std::filesystem::path& path) {
      std::ifstream file {path};
      REQUIRE(file.is_open());

      std::ostringstream buffer;
      buffer << file.rdbuf();
      return buffer.str();
   }

   std::vector<uint8_t> readBinaryFile(const std::filesystem::path& path) {
      std::ifstream file {path, std::ios::binary};
      REQUIRE(file.is_open());

      return std::vector<uint8_t> {
         std::istreambuf_iterator<char>(file),
         std::istreambuf_iterator<char>()
      };
   }
}

TEST_CASE("Netpbm saveImage default overload writes a P3 file") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "sample.ppm";

   raytracer::Image image {2, 1, 3};
   image.setPixel(raytracer::RGBColor {255, 0, 0}, 0, 0);
   image.setPixel(raytracer::RGBColor {0, 255, 0}, 0, 1);

   raytracer::netpbm::saveImage(image, outputPath.string());

   const std::string fileContent = readTextFile(outputPath);

   REQUIRE(fileContent == "P3\n2 1\n255\n255 0 0\n0 255 0\n");
}

TEST_CASE("Netpbm saveImage writes a P6 file when requested") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "sample_binary.ppm";

   raytracer::Image image {2, 1, 3};
   image.setPixel(raytracer::RGBColor {255, 0, 0}, 0, 0);
   image.setPixel(raytracer::RGBColor {0, 255, 0}, 0, 1);

   raytracer::netpbm::saveImage(image, outputPath.string(), raytracer::netpbm::P6);

   const std::vector<uint8_t> fileContent = readBinaryFile(outputPath);
   const std::vector<uint8_t> expectedContent {
      'P', '6', '\n',
      '2', ' ', '1', '\n',
      '2', '5', '5', '\n',
      255, 0, 0,
      0, 255, 0
   };

   REQUIRE(fileContent == expectedContent);
}

TEST_CASE("Netpbm saveImage rejects unsupported formats") {
   TemporaryDirectory temporaryDirectory;
   const auto outputPath = temporaryDirectory.path() / "unsupported.ppm";

   raytracer::Image image {1, 1, 3};

   REQUIRE_THROWS_AS(
      raytracer::netpbm::saveImage(image, outputPath.string(), raytracer::netpbm::P2),
      std::invalid_argument
   );
}

TEST_CASE("Netpbm loadImage loads an ASCII P3 image") {
   TemporaryDirectory temporaryDirectory;
   const auto inputPath = temporaryDirectory.path() / "input_p3.ppm";

   {
      std::ofstream file {inputPath};
      REQUIRE(file.is_open());
      file << "P3\n"
           << "2 1\n"
           << "255\n"
           << "255 0 0\n"
           << "0 255 0\n";
   }

   const raytracer::Image image = raytracer::netpbm::loadImage(
      inputPath.string(),
      raytracer::netpbm::P3
   );

   REQUIRE(image.getWidth() == 2);
   REQUIRE(image.getHeight() == 1);
   REQUIRE(image.getChannels() == 3);
   requirePixel(image.getPixel(0, 0), 255, 0, 0);
   requirePixel(image.getPixel(0, 1), 0, 255, 0);
}

TEST_CASE("Netpbm loadImage loads a binary P6 image") {
   TemporaryDirectory temporaryDirectory;
   const auto inputPath = temporaryDirectory.path() / "input_p6.ppm";

   {
      std::ofstream file {inputPath, std::ios::binary};
      REQUIRE(file.is_open());

      const std::vector<uint8_t> content {
         'P', '6', '\n',
         '2', ' ', '1', '\n',
         '2', '5', '5', '\n',
         255, 0, 0,
         0, 255, 0
      };

      file.write(
         reinterpret_cast<const char*>(content.data()),
         static_cast<std::streamsize>(content.size())
      );
   }

   const raytracer::Image image = raytracer::netpbm::loadImage(
      inputPath.string(),
      raytracer::netpbm::P6
   );

   REQUIRE(image.getWidth() == 2);
   REQUIRE(image.getHeight() == 1);
   REQUIRE(image.getChannels() == 3);
   requirePixel(image.getPixel(0, 0), 255, 0, 0);
   requirePixel(image.getPixel(0, 1), 0, 255, 0);
}

TEST_CASE("Netpbm loadImage rejects unsupported formats") {
   REQUIRE_THROWS_AS(
      raytracer::netpbm::loadImage("unused.ppm", raytracer::netpbm::P2),
      std::invalid_argument
   );
}

TEST_CASE("Netpbm loadImage rejects mismatched magic numbers") {
   TemporaryDirectory temporaryDirectory;
   const auto inputPath = temporaryDirectory.path() / "wrong_magic.ppm";

   {
      std::ofstream file {inputPath};
      REQUIRE(file.is_open());
      file << "P6\n"
           << "1 1\n"
           << "255\n"
           << "0 0 0\n";
   }

   REQUIRE_THROWS_AS(
      raytracer::netpbm::loadImage(inputPath.string(), raytracer::netpbm::P3),
      std::runtime_error
   );
}
