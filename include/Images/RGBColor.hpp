#ifndef RGB_COLOR_HPP
#define RGB_COLOR_HPP

#include <cstdint>

namespace raytracer {
   enum RGBChannel {
      RED = 0,
      GREEN = 1,
      BLUE = 2
   };

   class RGBColor {
      private:
         uint8_t _red;
         uint8_t _green;
         uint8_t _blue;
         
         /** Private helper function */
         uint8_t clampChannel(int value) const;
      public:
         /** Constructors */
         constexpr RGBColor(): _red(0), _green(0), _blue(0) {}
         constexpr RGBColor(
            uint8_t red, uint8_t green, uint8_t blue
         ): _red(red), _green(green), _blue(blue) {}

         /** Destructor */
         ~RGBColor() = default;

         /** Getters and Setters */
         uint8_t getRed() const;
         uint8_t getGreen() const;
         uint8_t getBlue() const;
         void setRed(uint8_t red);
         void setGreen(uint8_t green);
         void setBlue(uint8_t blue);

         /** Access operator */
         uint8_t operator[](const RGBChannel& channel) const;

         /** Operators */
         RGBColor operator+(const RGBColor& rhs) const;
         RGBColor& operator+=(const RGBColor& rhs);
         RGBColor operator-(const RGBColor& rhs) const;
         RGBColor& operator-=(const RGBColor& rhs);
         RGBColor operator*(double scale) const;
         RGBColor& operator*=(double scale);
         RGBColor operator/(double scale) const;
         RGBColor& operator/=(double scale);

         /** Conversion functions */
         RGBColor toGrayScale() const;
   };

   /** PIXELS CONSTANTS */
   constexpr const RGBColor PIXEL_BLACK {0, 0, 0};
   constexpr const RGBColor PIXEL_WHITE {255, 255, 255};
   constexpr const RGBColor PIXEL_RED {255, 0, 0};
   constexpr const RGBColor PIXEL_GREEN {0, 255, 0};
   constexpr const RGBColor PIXEL_BLUE {0, 0, 255};
   constexpr const RGBColor PIXEL_YELLOW {255, 255, 0};
   constexpr const RGBColor PIXEL_MAGENTA {255, 0, 255};
   constexpr const RGBColor PIXEL_CYAN {0, 255, 255};
}

#endif // !RGB_COLOR_HPP