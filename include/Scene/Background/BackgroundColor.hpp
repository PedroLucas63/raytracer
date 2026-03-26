#ifndef BACKGROUND_COLOR_HPP
#define BACKGROUND_COLOR_HPP

#include "Image/RGBColor.hpp"
#include "Image/Image.hpp"
#include "Math/Point2.hpp"
#include <cstdint>
#include <array>
#include <stdexcept>
#include "Parser/ParserScene.hpp"
#include "Background.hpp"


namespace raytracer {  
   enum Corners{ TopLeft = 0, TopRight, BottomLeft, BottomRight };

   class BackgroundColor : public Background {
      private:
         std::array<RGBColor, 4> _corners;
         static RGBColor lerp(const RGBColor& A, const RGBColor& B, float t);
         float clampCoordinate(float coordinate) const;
      public:

         BackgroundColor(
            const RGBColor& tl, const RGBColor& tr,
            const RGBColor& bl, const RGBColor& br
         ) : _corners{tl, tr, bl, br} {}

         BackgroundColor(const ParamSets& params);

         ~BackgroundColor() {};

         static BackgroundColor solid(const RGBColor& color);
         static BackgroundColor horizontalGradient(const RGBColor& left, const RGBColor& right);
         static BackgroundColor verticalGradient(const RGBColor& top, const RGBColor& bottom);
         static BackgroundColor diagonalGradientTLBR(const RGBColor& tl, const RGBColor& br);
         static BackgroundColor diagonalGradientTRBL(const RGBColor& tr, const RGBColor& bl);


         RGBColor sampleUV(float u, float v) const;

         RGBColor samplePixel(uint16_t col, uint16_t row,
                              uint16_t width, uint16_t height) const;
            
         RGBColor samplePixel(Point2 position, uint16_t width, uint16_t height) const;
         
         Image toImage(uint16_t width, uint16_t height) const;
   };
};

#endif //BACKGROUND_COLOR_HPP