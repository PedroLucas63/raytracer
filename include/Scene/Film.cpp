#include "Film.hpp"
#include "Utils/ImageUtils.hpp"

namespace raytracer {
   /** Constructors */
   Film Film::load(std::string filename) {
      Image image = ImageUtils::loadImage(filename);
      return Film(image, filename);
   }

   /** Getters and Setters */
   uint16_t Film::getWidth() const {
      return _image.getWidth();
   }
   uint16_t Film::getHeight() const {
      return _image.getHeight();
   }
   uint8_t Film::getChannels() const {
      return _image.getChannels();
   }
   RGBColor Film::getPixel(uint16_t row, uint16_t col) const {
      return _image.getPixel(row, col);
   }
   std::string Film::getFilename() const {
      return _filename;
   }
   Image Film::getImage() const {
      return _image;
   }
   FilmType Film::getType() const {
      return _type;
   }
   void Film::setFilename(const std::string& filename) {
      _filename = filename;
   }
   void Film::setPixel(const RGBColor& pixel, uint16_t row, uint16_t col) {
      _image.setPixel(pixel, row, col);
   }

   /** Access Operator */
   RGBColor Film::operator()(uint16_t row, uint16_t col) {
      return _image(row, col);
   }
   RGBColor Film::operator[](Point2 position) {
      return _image[position];
   }

   /** Conversion functions */
   Film Film::toGrayScale() const {
      Image grayImage = _image.toGrayScale();
      return Film(grayImage, _filename);
   }

   /** Save the film to a file */
   void Film::save() const {
      if (_filename.empty()) {
         throw std::runtime_error("Filename is not set. Cannot save the film.");
      }
      ImageUtils::saveImage(_image, _filename);
   }
}