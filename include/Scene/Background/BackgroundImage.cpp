#include "Scene/Background/BackgroundImage.hpp"
#include <cmath>
#include "Utils/ImageUtils.hpp"

namespace raytracer {
   RGBColor BackgroundImage::samplePixelByUV(float u, float v) const {
      float col = u * (_image.getWidth() - 1);
      float row = v * (_image.getHeight() - 1);

      uint16_t colLow = static_cast<uint16_t>(std::floor(col));
      uint16_t rowLow = static_cast<uint16_t>(std::floor(row));
      uint16_t colHigh = static_cast<uint16_t>(std::ceil(col));
      uint16_t rowHigh = static_cast<uint16_t>(std::ceil(row));

      RGBColor pixel00 = _image.getPixel(rowLow, colLow);
      RGBColor pixel01 = _image.getPixel(rowLow, colHigh);
      RGBColor pixel10 = _image.getPixel(rowHigh, colLow);
      RGBColor pixel11 = _image.getPixel(rowHigh, colHigh);

      float s = col - colLow;
      float t = row - rowLow;

      RGBColor color0 = pixel01.interpolate(pixel00, s);
      RGBColor color1 = pixel11.interpolate(pixel10, s);

      return color1.interpolate(color0, t);
   }
   
   float BackgroundImage::clampCoordinate(float coordinate) const {
      return std::clamp(coordinate, 0.0f, 1.0f);
   }

   BackgroundImage::BackgroundImage(const ParamSets& params): _image(1, 1, 3) {
      auto it = params.find("background");
      if (it == params.end()) {
         throw std::invalid_argument("Background parameters not found");
      }

      const ParamSet& bgParams = it->second;
      auto filename = bgParams.retrieve<std::string>("filename");
      _image = ImageUtils::loadImage(filename);

      _isSpherical = bgParams.retrieveOrDefault<bool>("spherical", false);
   }

   RGBColor BackgroundImage::sampleUV(float u, float v) const {
      u = clampCoordinate(u);
      v = clampCoordinate(v);
      return samplePixelByUV(u, v);
   }

   RGBColor BackgroundImage::samplePixel(
      uint16_t col, uint16_t row,
      uint16_t width, uint16_t height
   ) const {
      float u = static_cast<float>(col) / (width - 1);
      float v = static_cast<float>(row) / (height - 1);
      return sampleUV(u, v);
   }

   RGBColor BackgroundImage::samplePixel(
      Point2 position, uint16_t width, uint16_t height
   ) const {
      float u = static_cast<float>(position.getX()) / (width - 1);
      float v = static_cast<float>(position.getY()) / (height - 1);
      return sampleUV(u, v);
   }

   Image BackgroundImage::toImage(uint16_t width, uint16_t height) const {
      return _image;
   }

   std::shared_ptr<Background> BackgroundImage::clone() const {
      return std::make_shared<BackgroundImage>(*this);
   }

   RGBColor BackgroundImage::sampleDirection(const Vector3& direction) const {
      auto dir = direction.normalize();
      float x = dir.getX();
      float y = std::clamp(static_cast<float>(dir.getY()), -1.0f, 1.0f);
      float z = dir.getZ();

      float u = 0.5f + std::atan2(-z, x) / (2.0f * M_PI);
      float v = 0.5f - std::asin(y) / M_PI;

      u = u - std::floor(u);
      v = std::clamp(v, 0.0f, 1.0f);

      return samplePixelByUV(u, v);
   }

   bool BackgroundImage::isSpherical() const {
      return _isSpherical;
   }
}