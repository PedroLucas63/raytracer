#ifndef FILM_HPP
#define FILM_HPP

#include "Image/Image.hpp"
#include "Parser/ParserScene.hpp"
#include <string>

namespace raytracer {
   enum FilmType {
      TypeImage,
   };

   class Film {
      private:
         Image _image;
         FilmType _type = FilmType::TypeImage;
         std::string _filename;
      
      public:
         /** Constructors */
         Film(const Image& image): _image(image) {}
         Film(const Image& image, std::string filename): _image(image), _filename(filename) {}
         Film(uint16_t width, uint16_t height, uint8_t channels):
            _image(width, height, channels) {}
         Film(RGBColor* pixels, uint16_t width, uint16_t height, uint8_t channels):
            _image(pixels, width, height, channels) {}
         Film(uint16_t width, uint16_t height, uint8_t channels, std::string filename):
            _image(width, height, channels), _filename(filename) {}
         Film(RGBColor* pixels, uint16_t width, uint16_t height, uint8_t channels, std::string filename):
            _image(pixels, width, height, channels), _filename(filename) {}
         Film(const ParamSets& params);
         static Film load(std::string filename);

         /** Destructor */
         ~Film() = default;

         /** Getters and Setters */
         uint16_t getWidth() const;
         uint16_t getHeight() const;
         uint8_t getChannels() const;
         RGBColor getPixel(uint16_t row, uint16_t col) const;
         std::string getFilename() const;
         Image getImage() const;
         FilmType getType() const;
         void setImage(const Image& image);
         void setFilename(const std::string& filename);
         void setPixel(const RGBColor& pixel, uint16_t row, uint16_t col);

         /** Access Operator */
         RGBColor operator()(uint16_t row, uint16_t col);
         RGBColor operator[](Point2 position);

         /** Conversion functions */
         Film toGrayScale() const;

         /** Save the film to a file */
         void save() const;
         void save(const std::string& filename) const;
   };
}

#endif // !FILM_HPP