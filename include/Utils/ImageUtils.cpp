#include "ImageUtils.hpp"
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdexcept>

namespace raytracer {
   Image loadImage(std::string filename) {
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

   void saveImage(const Image& image, std::string filename, ImageType type) {
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
}