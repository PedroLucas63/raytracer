#include "Background.hpp"
#include <algorithm>  

namespace raytracer{

   /** Private helper methods */
   RGBPixel Background::lerp(const RGBPixel& A, const RGBPixel& B, float t) const {
      if (t < 0.0f || t > 1.0f)
         throw std::invalid_argument("t must be in [0, 1]");
      return A * (1.0f - t) + B * t;
   }

   float Background::clampCoordinate(float coordinate) const {
      return std::clamp(coordinate, 0.0f, 1.0f);
   }

   /** Sampling methods */
   RGBPixel Background::sampleUV(float u, float v) const {
      u = clampCoordinate(u);
      v = clampCoordinate(v);
      RGBPixel top = lerp(_corners[TopLeft], _corners[TopRight], u);
      RGBPixel bottom = lerp(_corners[BottomLeft], _corners[BottomRight], u);
      return lerp(top, bottom, v);
   }

   RGBPixel Background::samplePixel(uint16_t col, uint16_t row,
                                    uint16_t width, uint16_t height) const {
      float u = static_cast<float>(col) / (width - 1);
      float v = static_cast<float>(row) / (height - 1);
      return sampleUV(u, v);
   }

};