#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "RGBPixel.hpp"
#include <cstdint>
#include <array>
#include <stdexcept>

namespace raytracer {  

   enum Corners{ TopLeft = 0, TopRight, BottomLeft, BottomRight };

   class Background{
      private:
         std::array<RGBPixel, 4> _corners;

         RGBPixel lerp(const RGBPixel& A, const RGBPixel& B, float t) const;

         float clampCoordinate(float coordinate) const;

      public:

         Background(
            const RGBPixel& tl, const RGBPixel& tr,
            const RGBPixel& bl, const RGBPixel& br
         ) : _corners{tl, tr, bl, br} {}

         ~Background(){};

         static Background solid(const RGBPixel& color) {
            return Background(color, color, color, color);
         }

         RGBPixel sampleUV(float u, float v) const;

         RGBPixel samplePixel(uint16_t col, uint16_t row,
                              uint16_t width, uint16_t height) const;
   };
};

#endif //BACKGROUND_HPP