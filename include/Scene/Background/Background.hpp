#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Image/RGBColor.hpp"
#include "Math/Point2.hpp"
#include "Image/Image.hpp"
#include "Parser/ParserScene.hpp"

namespace raytracer {
   class Background {
      public:
         virtual ~Background() = default;
         virtual RGBColor sampleUV(float u, float v) const = 0;
         virtual RGBColor samplePixel(uint16_t col, uint16_t row,
                                      uint16_t width, uint16_t height) const = 0;
         virtual RGBColor samplePixel(Point2 position, uint16_t width, uint16_t height) const = 0;
         virtual Image toImage(uint16_t width, uint16_t height) const = 0;
   };
};

#endif //BACKGROUND_HPP