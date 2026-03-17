#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include "Image/Image.hpp"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

namespace raytracer {
   enum ImageType {
      PNG,
      BMP,
      TGA,
      JPG,
      NETPBM_P3,
      NETPBM_P6
   };

   class ImageUtils {
   private:
      ImageUtils() = default; // Private constructor to prevent instantiation

      static std::string getFileNameExtension(const std::string& filename);
      static ImageType determineImageType(const std::string& filename);

      static Image loadNetpbmImage(const std::string& filename, ImageType type);
      static Image loadStandardImage(const std::string& filename);

      static void saveStandardImage(const Image& image, const std::string& filename, ImageType type);
      static void saveNetpbmImage(const Image& image, const std::string& filename, ImageType type);
   public:
      static Image loadImage(const std::string filename);
      static void saveImage(const Image& image, const std::string filename, ImageType type = PNG);
   };
}

#endif // !IMAGE_UTILS_HPP