#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include "Images/Image.hpp"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

namespace raytracer {
   enum ImageType {
      PNG,
      BMP,
      TGA,
      JPG
   };

   Image loadImage(std::string filename);
   void saveImage(const Image& image, std::string filename, ImageType type = PNG);
}

#endif // !IMAGE_UTILS_HPP