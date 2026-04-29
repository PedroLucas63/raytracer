#include "RGBColor.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace raytracer {

   float RGBColor::clampChannel(float value) {
      return std::max(0.0f, std::min(value, 1.0f));
   }

   /** fromNormalized  stores float [0,1] directly, clamping for safety */
   RGBColor RGBColor::fromNormalized(float red, float green, float blue) {
      RGBColor c;
      c._red   = clampChannel(red);
      c._green = clampChannel(green);
      c._blue  = clampChannel(blue);
      return c;
   }

   RGBColor RGBColor::lerp(const RGBColor& a, const RGBColor& b, float t) {
      if (t < 0.0f || t > 1.0f)
         throw std::invalid_argument("t must be in [0, 1]");
      return b.interpolate(a, t);
   }

   /** Getters — return [0, 255] by multiplying internal float by 255 */
   uint8_t RGBColor::getRed()   const { return static_cast<uint8_t>(std::round(_red   * 255.0f)); }
   uint8_t RGBColor::getGreen() const { return static_cast<uint8_t>(std::round(_green * 255.0f)); }
   uint8_t RGBColor::getBlue()  const { return static_cast<uint8_t>(std::round(_blue  * 255.0f)); }

   /** Getters — return normalized float [0, 1] directly from storage */
   float RGBColor::getRedNormalized()   const { return _red;   }
   float RGBColor::getGreenNormalized() const { return _green; }
   float RGBColor::getBlueNormalized()  const { return _blue;  }

   /** Setters — receive [0, 255], divide by 255 to store */
   void RGBColor::setRed(uint8_t red)     { _red   = red   / 255.0f; }
   void RGBColor::setGreen(uint8_t green) { _green = green / 255.0f; }
   void RGBColor::setBlue(uint8_t blue)   { _blue  = blue  / 255.0f; }

   /** Setters — receive normalized float, clamp and store */
   void RGBColor::setRedNormalized(float red)     { _red   = clampChannel(red);   }
   void RGBColor::setGreenNormalized(float green) { _green = clampChannel(green); }
   void RGBColor::setBlueNormalized(float blue)   { _blue  = clampChannel(blue);  }

   /** Access operator — returns [0, 255] */
   uint8_t RGBColor::operator[](const RGBChannel& channel) const {
      switch (channel) {
         case RGBChannel::RED:   return getRed();
         case RGBChannel::GREEN: return getGreen();
         case RGBChannel::BLUE:  return getBlue();
      }
      throw std::out_of_range("Invalid RGB channel");
   }

   /** Operators — all work in float [0,1] space, clamp result */
   RGBColor RGBColor::operator+(const RGBColor& rhs) const {
      return fromNormalized(_red + rhs._red, _green + rhs._green, _blue + rhs._blue);
   }
   RGBColor& RGBColor::operator+=(const RGBColor& rhs) {
      _red   = clampChannel(_red   + rhs._red);
      _green = clampChannel(_green + rhs._green);
      _blue  = clampChannel(_blue  + rhs._blue);
      return *this;
   }

   RGBColor RGBColor::operator-(const RGBColor& rhs) const {
      return fromNormalized(_red - rhs._red, _green - rhs._green, _blue - rhs._blue);
   }
   RGBColor& RGBColor::operator-=(const RGBColor& rhs) {
      _red   = clampChannel(_red   - rhs._red);
      _green = clampChannel(_green - rhs._green);
      _blue  = clampChannel(_blue  - rhs._blue);
      return *this;
   }

   RGBColor RGBColor::operator*(double scale) const {
      if (scale < 0)
         throw std::invalid_argument("Scale factor must be a positive value");
      return fromNormalized(
         static_cast<float>(_red   * scale),
         static_cast<float>(_green * scale),
         static_cast<float>(_blue  * scale)
      );
   }
   RGBColor& RGBColor::operator*=(double scale) {
      if (scale < 0)
         throw std::invalid_argument("Scale factor must be a positive value");
      _red   = clampChannel(static_cast<float>(_red   * scale));
      _green = clampChannel(static_cast<float>(_green * scale));
      _blue  = clampChannel(static_cast<float>(_blue  * scale));
      return *this;
   }

   RGBColor RGBColor::operator/(double scale) const {
      if (scale <= 0)
         throw std::invalid_argument("Scale factor must be a positive value");
      return fromNormalized(
         static_cast<float>(_red   / scale),
         static_cast<float>(_green / scale),
         static_cast<float>(_blue  / scale)
      );
   }

   RGBColor& RGBColor::operator/=(double scale) {
      if (scale <= 0)
         throw std::invalid_argument("Scale factor must be a positive value");
      _red   = clampChannel(static_cast<float>(_red   / scale));
      _green = clampChannel(static_cast<float>(_green / scale));
      _blue  = clampChannel(static_cast<float>(_blue  / scale));
      return *this;
   }

   /** Conversion functions */
   RGBColor RGBColor::toGrayScale() const {
      float grayValue = 
         0.299f * _red +
         0.587f * _green +
         0.114f * _blue
         ;
      return fromNormalized(grayValue, grayValue, grayValue);
   }

   RGBColor RGBColor::interpolate(const RGBColor& other, double t) const {
      if (t < 0 || t > 1)
         throw std::invalid_argument("t must be in [0, 1]");
      return fromNormalized(
         static_cast<float>(_red   * t + other._red   * (1.0 - t)),
         static_cast<float>(_green * t + other._green * (1.0 - t)),
         static_cast<float>(_blue  * t + other._blue  * (1.0 - t))
      );
   }
}