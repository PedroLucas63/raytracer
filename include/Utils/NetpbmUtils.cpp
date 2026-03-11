#include "NetpbmUtils.hpp"
#include <cctype>
#include <fstream>

namespace raytracer {
   namespace netpbm {
      Image loadImage(std::string pathfile, NetpbmType type) {
         // TODO: Allow comments on the image
         // TODO: Allow other image types
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
         int width, height, maxColorValue;

         file >> width >> height >> maxColorValue;

         if (maxColorValue != 255)
            throw std::runtime_error("Unsupported max color value: " + std::to_string(maxColorValue) + ". Only 255 is supported.");

         if (type == P6) {
            char separator;
            file.get(separator);

            if (!file || !std::isspace(static_cast<unsigned char>(separator)))
               throw std::runtime_error("Invalid P6 header separator");
         }

         // READ PIXELS
         Image image(width, height, 3);
         for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
               if (type == P3) {
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
               RGBColor pixel = image.getPixel(row, col);
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