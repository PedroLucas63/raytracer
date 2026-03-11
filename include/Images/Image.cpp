#include "Image.hpp"
#include <stddef.h>
#include <stdexcept>

namespace raytracer {
   /** Private setters methods */
   void Image::setWidth(uint16_t width) {
      if (width == 0) 
         throw std::invalid_argument("Width must be a positive value");
      _width = width;
   }
   void Image::setHeight(uint16_t height) {
      if (height == 0) 
         throw std::invalid_argument("Height must be a positive value");
      _height = height;
   }
   void Image::setChannels(uint8_t channels) {
      if (channels != 1 && channels != 3)
         throw std::invalid_argument("Channels must be either 1 (grayscale) or 3 (RGB)");
      _channels = channels;
   }

   /** Constructors */
   Image::Image(uint16_t width, uint16_t height, uint8_t channels) {
      setWidth(width);
      setHeight(height);
      setChannels(channels);
      _pixels = new RGBColor[width * height];
   }
   Image::Image(
      RGBColor* pixels, 
      uint16_t width, 
      uint16_t height, 
      uint8_t channels
   ): Image(width, height, channels) {
      size_t numPixels = width * height;
      for (int i = 0; i < numPixels; i++) {
         _pixels[i] = pixels[i];
      }
   }

   /** Destructor */
   Image::~Image() {
      delete[] _pixels;
   }

   /** Getters and Setters */
   uint16_t Image::getWidth() const {
      return _width;
   }
   uint16_t Image::getHeight() const {
      return _height;
   }
   uint8_t Image::getChannels() const {
      return _channels;
   }
   RGBColor Image::getPixel(uint16_t row, uint16_t col) const {
      size_t pos = row * _width + col;
      
      if (_channels == 1) return _pixels[pos].toGrayScale();
      else return _pixels[pos];
   }
   void Image::setPixel(const RGBColor& pixel, uint16_t row, uint16_t col) {
      if (col >= _width)
         throw std::out_of_range("Column index out of bounds");
      if (row >= _height)
         throw std::out_of_range("Row index out of bounds");

      size_t pos = row * _width + col;
      _pixels[pos] = pixel;
   }

   /** Access Operator */
   RGBColor Image::operator()(uint16_t row, uint16_t col) {
      return getPixel(row, col);
   }

   /** Conversion functions */
   Image Image::toGrayScale() const {
      Image grayImage(_width, _height, 3); // TODO: This will create a grayscale image with 3 channels (RGB), but all channels will have the same value.
      for (uint16_t row = 0; row < _height; row++) {
         for (uint16_t col = 0; col < _width; col++) {
            RGBColor pixel = getPixel(row, col);
            RGBColor grayPixel = pixel.toGrayScale();
            grayImage.setPixel(grayPixel, row, col);
         }
      }
      return grayImage;
   }
}