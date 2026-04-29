#include "Objects/Light/Light.hpp"

namespace raytracer {
   Light::Light(const RGBColor intensity, const Vector3 scale) {
      setIntensity(intensity);
      setScale(scale);
   }

   Light::Light(const ParamSet& params) : _intensity(0.0, 0.0, 0.0) {
      if (params.has("I")) {
         setIntensity(params.retrieve<RGBColor>("I"));
      } else {
         throw std::invalid_argument("Light requires an 'I' parameter of type RGBColor");
      }

      if (params.has("scale")) {
         setScale(params.retrieve<Vector3>("scale"));
      }

   }

   Vector3 Light::getIntensity() const {
      return _intensity;
   }

   void Light::setIntensity(const RGBColor& intensity) {
      _intensity = Vector3(
         intensity.getRedNormalized(),
         intensity.getGreenNormalized(),
         intensity.getBlueNormalized()
      );
   }

   void Light::setScale(const Vector3& scale) {
      _intensity = Vector3(
         _intensity.getX() * scale.getX(),
         _intensity.getY() * scale.getY(),
         _intensity.getZ() * scale.getZ()
      );
   }
}