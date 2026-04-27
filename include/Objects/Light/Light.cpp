#include "Objects/Light/Light.hpp"

namespace raytracer {
   Light::Light(const RGBColor intensity, const Vector3 scale) :
      _intensity(intensity), _scale(scale) {}

   Light::Light(const ParamSet& params) : _intensity(), _scale() {
      if (params.has("I")) {
         _intensity = params.retrieve<RGBColor>("I");
      } else {
         throw std::invalid_argument("Light requires an 'I' parameter of type RGBColor");
      }

      if (params.has("scale")) {
         setScale(params.retrieve<Vector3>("scale"));
      } else {
         _scale = VECTOR3_ONE;
      }
   }

   RGBColor Light::getIntensity() const {
      return _intensity;
   }

   void Light::setIntensity(const RGBColor& intensity) {
      _intensity = intensity;
   }

   Vector3 Light::getScale() const {
      return _scale;
   }

   void Light::setScale(const Vector3& scale) {
      _scale = scale.clamp(0.0, 1.0);
   }
}