#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "RGBPixel.hpp"
#include <cstdint>
#include <utility>

namespace raytracer {
   class Image {
      private:
         RGBPixel* _pixels;
         uint16_t _width;
         uint16_t _height;
         uint8_t _channels;

         /** Private setters methods */
         void setWidth(uint16_t width);
         void setHeight(uint16_t height);
         void setChannels(uint8_t channels);
      public:
         /** Constructors */
         Image(uint16_t width, uint16_t height, uint8_t channels);
         Image(RGBPixel* pixels, uint16_t width, uint16_t height, uint8_t channels);

         /** Destructor */
         ~Image();

         /** Getters and Setters */
         uint16_t getWidth() const;
         uint16_t getHeight() const;
         uint8_t getChannels() const;
         RGBPixel getPixel(uint16_t row, uint16_t col) const;
         void setPixel(const RGBPixel& pixel, uint16_t row, uint16_t col);

         /** Access Operator */
         RGBPixel operator()(uint16_t row, uint16_t col);

         /** Conversion functions */
         Image toGrayScale() const;
   };
};

#endif // !IMAGE_HPP