#ifndef BACKGROUND_IMAGE_HPP
#define BACKGROUND_IMAGE_HPP

#include "Background.hpp"
#include "Image/Image.hpp"
#include "Parser/ParserScene.hpp"

namespace raytracer {
   class BackgroundImage : public Background {
      private:
         Image _image;
         float clampCoordinate(float coordinate) const;
         RGBColor samplePixelByUV(float u, float v) const;
      public:
         BackgroundImage(const Image& image): _image(image) {};
         BackgroundImage(const ParamSets& params);
         RGBColor sampleUV(float u, float v) const override;
         RGBColor samplePixel(uint16_t col, uint16_t row,
                              uint16_t width, uint16_t height) const override;
         RGBColor samplePixel(Point2 position, uint16_t width, uint16_t height) const override;
         Image toImage(uint16_t width, uint16_t height) const override;
   };
};

#endif // !BACKGROUND_IMAGE_HPP