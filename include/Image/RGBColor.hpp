#ifndef RGB_COLOR_HPP
#define RGB_COLOR_HPP

#include <cstdint>
#include <iterator>
#include <algorithm>
#include <stdexcept>

namespace raytracer {
   const constexpr int RGB_CHANELLS = 3;

   enum RGBChannel {
      RED = 0,
      GREEN = 1,
      BLUE = 2
   };

   class RGBColor {
      private:
         // Stored as normalized float [0, 1] — no clamp loss on intermediate operations
         float _red;
         float _green;
         float _blue;

         /** Clamps a float to [0, 1] */
         static float clampChannel(float value);

      public:
         /** Constructors */
         constexpr RGBColor() : _red(0.0f), _green(0.0f), _blue(0.0f) {}

         /** Takes values in [0, 255] range (classic uint8_t convention) */
         constexpr RGBColor(uint8_t red, uint8_t green, uint8_t blue)
            : _red(red / 255.0f), _green(green / 255.0f), _blue(blue / 255.0f) {}

         template <std::input_iterator Iter>
         requires std::same_as<std::iter_value_t<Iter>, uint8_t>
         constexpr RGBColor(Iter begin, Iter end) {
            auto it = begin;

            if (it == end) throw std::invalid_argument("RGBColor needs at least 3 values");
            _red = *it++ / 255.0f;

            if (it == end) throw std::invalid_argument("RGBColor needs at least 3 values");
            _green = *it++ / 255.0f;

            if (it == end) throw std::invalid_argument("RGBColor needs at least 3 values");
            _blue = *it++ / 255.0f;
         }

         static RGBColor fromNormalized(float red, float green, float blue);

         static RGBColor lerp(const RGBColor& a, const RGBColor& b, float t);

         /** Destructor */
         ~RGBColor() = default;
         
         /** Getters and Setters */

         uint8_t getRed()   const;
         uint8_t getGreen() const;
         uint8_t getBlue()  const;

         float getRedNormalized()   const;
         float getGreenNormalized() const;
         float getBlueNormalized()  const;

         void setRed(uint8_t red);
         void setGreen(uint8_t green);
         void setBlue(uint8_t blue);

         void setRedNormalized(float red);
         void setGreenNormalized(float green);
         void setBlueNormalized(float blue);

         /** Access operator */
         uint8_t operator[](const RGBChannel& channel) const;

         /** Operators */
         RGBColor  operator+(const RGBColor& rhs) const;
         RGBColor& operator+=(const RGBColor& rhs);
         RGBColor  operator-(const RGBColor& rhs) const;
         RGBColor& operator-=(const RGBColor& rhs);
         RGBColor  operator*(double scale) const;
         RGBColor& operator*=(double scale);
         RGBColor  operator/(double scale) const;
         RGBColor& operator/=(double scale);

         /** Conversion functions */
         RGBColor toGrayScale() const;
         RGBColor interpolate(const RGBColor& other, double t) const;
   };

   /** PIXEL CONSTANTS */
   constexpr const RGBColor PIXEL_BLACK   {  0,   0,   0};
   constexpr const RGBColor PIXEL_WHITE   {255, 255, 255};
   constexpr const RGBColor PIXEL_RED     {255,   0,   0};
   constexpr const RGBColor PIXEL_GREEN   {  0, 255,   0};
   constexpr const RGBColor PIXEL_BLUE    {  0,   0, 255};
   constexpr const RGBColor PIXEL_YELLOW  {255, 255,   0};
   constexpr const RGBColor PIXEL_MAGENTA {255,   0, 255};
   constexpr const RGBColor PIXEL_CYAN    {  0, 255, 255};
}

#endif // !RGB_COLOR_HPP