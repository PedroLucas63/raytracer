#include "ImageUtils.hpp"
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>

namespace raytracer {
   std::string ImageUtils::getFileNameExtension(const std::string& filename) {
      std::filesystem::path filePath(filename);
      if (!filePath.has_extension())
         throw std::invalid_argument("Filename does not have an extension: " + filename);
      
      return filePath.extension().string();
   }

   ImageType ImageUtils::determineImageType(const std::string& filename) {
      std::string extension = getFileNameExtension(filename);
      if (extension == ".png" || extension == ".PNG") {
         return PNG;
      } else if (extension == ".bmp" || extension == ".BMP") {
         return BMP;
      } else if (extension == ".tga" || extension == ".TGA") {
         return TGA;
      } else if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG") {
         return JPG;
      } else if (extension == ".ppm") {
         return NETPBM_P3; // Default to P3 for .ppm files
      } else if (extension == ".PPM") {
         return NETPBM_P6; // Default to P6 for .PPM files
      } else {
         throw std::invalid_argument("Unsupported file extension: " + extension);
      }
   }

   Image ImageUtils::loadStandardImage(const std::string& filename) {
      int width, height, channels;
      auto data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

      if (!data) throw std::runtime_error("Image cannot load.");

      Image image(width, height, channels);
      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            size_t index = (row * width + col) * channels;

            uint8_t red, green, blue;

            if (channels == 1) {
               red = green = blue = data[index];
            } else {
               red = data[index];
               green = data[index + 1];
               blue = data[index + 2];
            }

            image.setPixel(RGBColor(red, green, blue), row, col);
         }
      }

      stbi_image_free(data);
      return image;
   }

   Image ImageUtils::loadNetpbmImage(const std::string& filename, ImageType type) {
      // VALIDATE NETPBM TYPE
      if (type != NETPBM_P3 && type != NETPBM_P6)
         throw std::invalid_argument("Only P3 (ASCII RGB) and P6 (Binary RGB) Netpbm formats are supported");

      // OPEN IMAGE
      std::ifstream file;
      if (type == NETPBM_P3)
         file = std::ifstream(filename);
      else
         file = std::ifstream(filename, std::ios::binary);

      if (!file.is_open())
         throw std::runtime_error("Could not open file: " + filename);

      // GET MAGIC NUMBER
      std::string magicNumber;
      file >> magicNumber;

      if (
         (type == NETPBM_P3 && magicNumber != "P3") || (type == NETPBM_P6 && magicNumber != "P6")
      )
         throw std::runtime_error("Netpbm type mismatch: expected " + std::to_string(type) + ", got " + magicNumber);

      // GET WIDTH, HEIGHT AND MAX COLOR VALUE
      int width, height, maxColorValue;

      file >> width >> height >> maxColorValue;

      if (maxColorValue != 255)
         throw std::runtime_error("Unsupported max color value: " + std::to_string(maxColorValue) + ". Only 255 is supported.");

      if (type == NETPBM_P6) {
         char separator;
         file.get(separator);

         if (!file || !std::isspace(static_cast<unsigned char>(separator)))
            throw std::runtime_error("Invalid P6 header separator");
      }

      // READ PIXELS
      Image image(width, height, 3);
      for (int row = 0; row < height; ++row) {
         for (int col = 0; col < width; ++col) {
            if (type == NETPBM_P3) {
               int r, g, b;

               file >> r >> g >> b;

               if (!file)
                  throw std::runtime_error("Could not read P3 pixel data");

               image.setPixel(RGBColor(r, g, b), row, col);
            } else {
               uint8_t r, g, b;

               file.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));
               file.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));
               file.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));

               if (!file)
                  throw std::runtime_error("Could not read P6 pixel data");

               image.setPixel(RGBColor(r, g, b), row, col);
            }
         }
      }

      file.close();
      return image;
   }

   void ImageUtils::saveStandardImage(const Image& image, const std::string& filename, ImageType type) {
      char* data = new char[image.getWidth() * image.getHeight() * image.getChannels()];
      for (int row = 0; row < image.getHeight(); row++) {
         for (int col = 0; col < image.getWidth(); col++) {
            size_t index = (row * image.getWidth() + col) * image.getChannels();
            RGBColor pixel = image.getPixel(row, col);

            data[index] = pixel.getRed();
            if (image.getChannels() > 1) {
               data[index + 1] = pixel.getGreen();
               data[index + 2] = pixel.getBlue();
            }
         }
      }
      
      switch (type)
      {
      case PNG:
         stbi_write_png(
            filename.c_str(), 
            image.getWidth(), 
            image.getHeight(), 
            image.getChannels(), 
            data, 
            image.getWidth() * image.getChannels()
         );
         break;
      case BMP:
         stbi_write_bmp(
            filename.c_str(), 
            image.getWidth(), 
            image.getHeight(), 
            image.getChannels(), 
            data
         );
         break;
      case TGA:
         stbi_write_tga(
            filename.c_str(), 
            image.getWidth(), 
            image.getHeight(), 
            image.getChannels(), 
            data
         );
         break;
      case JPG:
         stbi_write_jpg(
            filename.c_str(), 
            image.getWidth(), 
            image.getHeight(), 
            image.getChannels(), 
            data, 
            100 // Quality (0-100)
         );
         break;
      default:
         throw std::invalid_argument("Unsupported image type.");
         break;
      }
   }

   void ImageUtils::saveNetpbmImage(const Image& image, const std::string& filename, ImageType type) {
      // VALIDATE TYPE
      if (type != NETPBM_P3 && type != NETPBM_P6)
         throw std::invalid_argument("Only P3 (ASCII RGB) and P6 (Binary RGB) Netpbm formats are supported");
      
      // OPEN FILE
      std::ofstream file;
      if (type == NETPBM_P3)
         file = std::ofstream(filename);
      else
         file = std::ofstream(filename, std::ios::binary);
      
      if (!file.is_open())
         throw std::runtime_error("Could not open file: " + filename);
      
      // WRITE MAGIC NUMBER
      file << (type == NETPBM_P3 ? "P3" : "P6") << "\n";

      // WRITE WIDTH, HEIGHT AND MAX COLOR VALUE
      file << image.getWidth() << " " << image.getHeight() << "\n255\n";

      // WRITE PIXELS
      for (int row = 0; row < image.getHeight(); ++row) {
         for (int col = 0; col < image.getWidth(); ++col) {
            RGBColor pixel = image.getPixel(row, col);
            uint8_t r = pixel.getRed();
            uint8_t g = pixel.getGreen();
            uint8_t b = pixel.getBlue();

            if (type == NETPBM_P3) {
               file << static_cast<int>(r) << " " 
                     << static_cast<int>(g) << " " 
                     << static_cast<int>(b) << "\n";
            } else {
               file.write(reinterpret_cast<char*>(&r), sizeof(uint8_t));
               file.write(reinterpret_cast<char*>(&g), sizeof(uint8_t));
               file.write(reinterpret_cast<char*>(&b), sizeof(uint8_t));
            }
         }
      }

      file.close();
   }

   Image ImageUtils::loadImage(const std::string filename) {
      ImageType type = determineImageType(filename);
      if (type == NETPBM_P3 || type == NETPBM_P6) {
         return loadNetpbmImage(filename, type);
      } else {
         return loadStandardImage(filename);
      }
   }

   void ImageUtils::saveImage(const Image& image, const std::string filename, ImageType type) {
      if (type == NETPBM_P3 || type == NETPBM_P6) {
         saveNetpbmImage(image, filename, type);
      } else {
         saveStandardImage(image, filename, type);
      }
   }
}