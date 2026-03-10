#include "NetpbmUtils.hpp"
#include <fstream>

namespace raytracer {
   namespace netpbm {
      Image loadImage(std::string pathfile, NetpbmType type) {
         // VALIDATE TYPE
         if (type != P3 && type != P6)
            throw std::invalid_argument("Only P3 (ASCII RGB) and P6 (Binary RGB) Netpbm formats are supported");

         // OPEN IMAGE
         std::ifstream file;
         if (type == P3)
            file = std::ifstream(pathfile);
         else
            file = std::ifstream(pathfile, std::ios::binary);

         if (!file.is_open())
            throw std::runtime_error("Could not open file: " + pathfile);

         // GET MAGIC NUMBER
         std::string magicNumber;
         file >> magicNumber;

         if (
            (type == P3 && magicNumber != "P3") || (type == P6 && magicNumber != "P6")
         )
            throw std::runtime_error("Netpbm type mismatch: expected " + std::to_string(type) + ", got " + magicNumber);

         // GET WIDTH, HEIGHT AND MAX COLOR VALUE
         uint16_t width, height;
         uint8_t maxColorValue;

         file >> width >> height >> maxColorValue;

         if (maxColorValue != 255)
            throw std::runtime_error("Unsupported max color value: " + std::to_string(maxColorValue) + ". Only 255 is supported.");

         // READ PIXELS
         Image image(width, height, 3);
         for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
               uint8_t r, g, b;

               if (type == P3) {
                  file >> r >> g >> b;
               } else {
                  file.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));
                  file.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));
                  file.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));
               }

               image.setPixel(RGBPixel(r, g, b), row, col);
            }
         }

         file.close();
         return image;
      }

      void saveImage(const Image& image, std::string filepath) {
         saveImage(image, filepath, P3);
      }

      void saveImage(const Image& image, std::string filepath, NetpbmType type) {
         // VALIDATE TYPE
         if (type != P3 && type != P6)
            throw std::invalid_argument("Only P3 (ASCII RGB) and P6 (Binary RGB) Netpbm formats are supported");
         
         // OPEN FILE
         std::ofstream file;
         if (type == P3)
            file = std::ofstream(filepath);
         else
            file = std::ofstream(filepath, std::ios::binary);
         
         if (!file.is_open())
            throw std::runtime_error("Could not open file: " + filepath);
         
         // WRITE MAGIC NUMBER
         file << (type == P3 ? "P3" : "P6") << "\n";

         // WRITE WIDTH, HEIGHT AND MAX COLOR VALUE
         file << image.getWidth() << " " << image.getHeight() << "\n255\n";

         // WRITE PIXELS
         for (int row = 0; row < image.getHeight(); ++row) {
            for (int col = 0; col < image.getWidth(); ++col) {
               RGBPixel pixel = image.getPixel(row, col);
               uint8_t r = pixel.getRed();
               uint8_t g = pixel.getGreen();
               uint8_t b = pixel.getBlue();

               if (type == P3) {
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
   }
}