#include "RGBColor.hpp"
#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace raytracer {
   /** Private helper function */
   uint8_t RGBColor::clampChannel(int value) const {
      const int MIN_VALUE = std::numeric_limits<uint8_t>::min();
      const int MAX_VALUE = std::numeric_limits<uint8_t>::max();
      return static_cast<uint8_t>(
         std::max(MIN_VALUE, std::min(value, MAX_VALUE))
      );   
   }

   /** Getters and Setters */
   uint8_t RGBColor::getRed() const {
      return _red;
   }
   uint8_t RGBColor::getGreen() const {
      return _green;
   }
   uint8_t RGBColor::getBlue() const {
      return _blue;
   }
   void RGBColor::setRed(uint8_t red) {
      _red = red;
   }
   void RGBColor::setGreen(uint8_t green) {
      _green = green;
   }
   void RGBColor::setBlue(uint8_t blue) {
      _blue = blue;
   }

   /** Access operator */
   uint8_t RGBColor::operator[](const RGBChannel& channel) const {
      switch (channel)
      {
      case RGBChannel::RED:
         return _red;
      case RGBChannel::GREEN:
         return _green;
      case RGBChannel::BLUE:
         return _blue;
      }

      throw std::out_of_range("Invalid RGB channel");
   }

   /** Operators */
   RGBColor RGBColor::operator+(const RGBColor& rhs) const {
      return RGBColor(
         clampChannel(_red + rhs._red),
         clampChannel(_green + rhs._green),
         clampChannel(_blue + rhs._blue)
      );
   }
   RGBColor& RGBColor::operator+=(const RGBColor& rhs) {
      _red = clampChannel(_red + rhs._red);
      _green = clampChannel(_green + rhs._green);
      _blue = clampChannel(_blue + rhs._blue);

      return *this;
   }
   RGBColor RGBColor::operator-(const RGBColor& rhs) const {
      return RGBColor(
         clampChannel(_red - rhs._red),
         clampChannel(_green - rhs._green),
         clampChannel(_blue - rhs._blue)
      );
   }
   RGBColor& RGBColor::operator-=(const RGBColor& rhs) {
      _red = clampChannel(_red - rhs._red);
      _green = clampChannel(_green - rhs._green);
      _blue = clampChannel(_blue - rhs._blue);

      return *this;
   }
   RGBColor RGBColor::operator*(double scale) const {
      if (scale < 0) 
         throw std::invalid_argument("Scale factor must be a positive value");

      return RGBColor(
         clampChannel(static_cast<int>(std::round(_red * scale))),
         clampChannel(static_cast<int>(std::round(_green * scale))),
         clampChannel(static_cast<int>(std::round(_blue * scale)))
      );
   }
   RGBColor& RGBColor::operator*=(double scale) {
      if (scale < 0) 
         throw std::invalid_argument("Scale factor must be a positive value");

      _red = clampChannel(static_cast<int>(std::round(_red * scale)));
      _green = clampChannel(static_cast<int>(std::round(_green * scale)));
      _blue = clampChannel(static_cast<int>(std::round(_blue * scale)));

      return *this;
   }
   RGBColor RGBColor::operator/(double scale) const {
      if (scale <= 0) 
         throw std::invalid_argument("Scale factor must be a positive value");

      return RGBColor(
         clampChannel(static_cast<int>(std::round(_red / scale))),
         clampChannel(static_cast<int>(std::round(_green / scale))),
         clampChannel(static_cast<int>(std::round(_blue / scale)))
      );
   }
   RGBColor& RGBColor::operator/=(double scale) {
      if (scale <= 0) 
         throw std::invalid_argument("Scale factor must be a positive value");

      _red = clampChannel(static_cast<int>(std::round(_red / scale)));
      _green = clampChannel(static_cast<int>(std::round(_green / scale)));
      _blue = clampChannel(static_cast<int>(std::round(_blue / scale)));

      return *this;
   }

   /** Conversion functions */
   RGBColor RGBColor::toGrayScale() const {
      uint8_t grayValue = std::round(
        0.299 * _red +
        0.587 * _green +
        0.114 * _blue
      );
      return RGBColor(grayValue, grayValue, grayValue);
   }
}