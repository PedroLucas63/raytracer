#include "Background.hpp"
#include <algorithm>  
#include <cmath>

namespace raytracer{

   /** Private helper methods */
   RGBColor Background::lerp(const RGBColor& A, const RGBColor& B, float t) {
      if (t < 0.0f || t > 1.0f)
         throw std::invalid_argument("t must be in [0, 1]");
      return B.interpolate(A, t);
   }

   float Background::clampCoordinate(float coordinate) const {
      return std::clamp(coordinate, 0.0f, 1.0f);
   }

   /** Static build methods */

   Background Background::solid(const RGBColor& color) {
      return Background(color, color, color, color);
   }
   Background Background::horizontalGradient(const RGBColor& left, const RGBColor& right) {
      return Background(left, right, left, right);
   }
   Background Background::verticalGradient(const RGBColor& top, const RGBColor& bottom) {
      return Background(top, top, bottom, bottom);
   }          
   Background Background::diagonalGradientTLBR(const RGBColor& tl, const RGBColor& br) {
      RGBColor other = lerp(tl, br, 0.5f);
      return Background(tl, other, other, br);
   }
   Background Background::diagonalGradientTRBL(const RGBColor& tr, const RGBColor& bl) {
      RGBColor other = lerp(tr, bl, 0.5f);
      return Background(other, tr, bl, other);
   }

   /** Sampling methods */
   RGBColor Background::sampleUV(float u, float v) const {
      u = clampCoordinate(u);
      v = clampCoordinate(v);
      RGBColor top = lerp(_corners[TopLeft], _corners[TopRight], u);
      RGBColor bottom = lerp(_corners[BottomLeft], _corners[BottomRight], u);
      return lerp(top, bottom, v);
   }

   RGBColor Background::samplePixel(uint16_t col, uint16_t row,
                                    uint16_t width, uint16_t height) const {
      float u = static_cast<float>(col) / (width - 1);
      float v = static_cast<float>(row) / (height - 1);
      return sampleUV(u, v);
   }

   RGBColor Background::samplePixel(
      Point2 position, uint16_t width, uint16_t height
   ) const {
      uint16_t col = static_cast<uint16_t>(std::round(position.getX()));
      uint16_t row = static_cast<uint16_t>(std::round(position.getY()));
      return samplePixel(col, row, width, height);
   }

   Image Background::toImage(uint16_t width, uint16_t height) const {
      Image image(width, height, 3);
      for (uint16_t row = 0; row < height; row++) {
         for (uint16_t col = 0; col < width; col++) {
            image.setPixel(samplePixel(col, row, width, height), row, col);
         }
      }
      return image;
   }

};