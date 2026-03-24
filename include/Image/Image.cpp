#include "Image.hpp"
#include <stddef.h>
#include <stdexcept>
#include <cmath>

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

   Image::Image(const Image& other):
      _pixels(nullptr),
      _width(other._width),
      _height(other._height),
      _channels(other._channels) {
      const size_t numPixels = static_cast<size_t>(_width) * _height;
      _pixels = new RGBColor[numPixels];

      for (size_t i = 0; i < numPixels; ++i) {
         _pixels[i] = other._pixels[i];
      }
   }

   Image::Image(Image&& other) noexcept:
      _pixels(other._pixels),
      _width(other._width),
      _height(other._height),
      _channels(other._channels) {
      other._pixels = nullptr;
      other._width = 0;
      other._height = 0;
      other._channels = 0;
   }

   Image& Image::operator=(const Image& other) {
      if (this == &other) {
         return *this;
      }

      const size_t numPixels = static_cast<size_t>(other._width) * other._height;
      RGBColor* newPixels = new RGBColor[numPixels];

      for (size_t i = 0; i < numPixels; ++i) {
         newPixels[i] = other._pixels[i];
      }

      delete[] _pixels;
      _pixels = newPixels;
      _width = other._width;
      _height = other._height;
      _channels = other._channels;

      return *this;
   }

   Image& Image::operator=(Image&& other) noexcept {
      if (this == &other) {
         return *this;
      }

      delete[] _pixels;
      _pixels = other._pixels;
      _width = other._width;
      _height = other._height;
      _channels = other._channels;

      other._pixels = nullptr;
      other._width = 0;
      other._height = 0;
      other._channels = 0;

      return *this;
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

   RGBColor Image::operator[](Point2 position) {
      uint16_t col = static_cast<uint16_t>(std::round(position.getX()));
      uint16_t row = static_cast<uint16_t>(std::round(position.getY()));
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