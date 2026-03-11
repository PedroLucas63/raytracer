#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "RGBColor.hpp"
#include "Image.hpp"
#include <cstdint>
#include <array>
#include <stdexcept>

namespace raytracer {  

   enum Corners{ TopLeft = 0, TopRight, BottomLeft, BottomRight };

   class Background{
      private:
         std::array<RGBColor, 4> _corners;

         static RGBColor lerp(const RGBColor& A, const RGBColor& B, float t);

         float clampCoordinate(float coordinate) const;

      public:

         Background(
            const RGBColor& tl, const RGBColor& tr,
            const RGBColor& bl, const RGBColor& br
         ) : _corners{tl, tr, bl, br} {}

         ~Background(){};

         static Background solid(const RGBColor& color);
         static Background horizontalGradient(const RGBColor& left, const RGBColor& right);
         static Background verticalGradient(const RGBColor& top, const RGBColor& bottom);
         static Background diagonalGradientTLBR(const RGBColor& tl, const RGBColor& br);
         static Background diagonalGradientTRBL(const RGBColor& tr, const RGBColor& bl);


         RGBColor sampleUV(float u, float v) const;

         RGBColor samplePixel(uint16_t col, uint16_t row,
                              uint16_t width, uint16_t height) const;
         
         Image toImage(uint16_t width, uint16_t height) const;
   };
};

#endif //BACKGROUND_HPP