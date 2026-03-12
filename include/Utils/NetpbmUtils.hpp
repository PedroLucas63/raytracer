#ifndef NETPBM_UTILS_HPP
#define NETPBM_UTILS_HPP

#include "Image/Image.hpp"
#include <string>

namespace raytracer {
   namespace netpbm {
      enum NetpbmType {
         P1,
         P2,
         P3,
         P4,
         P5,
         P6
      };

      Image loadImage(std::string pathfile, NetpbmType type);
      void saveImage(const Image& image, std::string filepath);
      void saveImage(const Image& image, std::string filepath, NetpbmType type);
   }
}

#endif // !NETPBM_UTILS_HPP