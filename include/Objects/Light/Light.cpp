#include "Objects/Light/Light.hpp"
#include "Utils/Utils.hpp"

namespace raytracer {
   Light::Light(const RGBColor intensity, const Vector3 scale) : _intensity(intensity) {
      setScale(scale);
   }

   Light::Light(const ParamSet& params) : _intensity() {
      if (params.has("I")) {
         _intensity = params.retrieve<RGBColor>("I");
      } else {
         throw std::invalid_argument("Light requires an 'I' parameter of type RGBColor");
      }

      if (params.has("scale")) {
         setScale(params.retrieve<Vector3>("scale"));
      }
   }

   RGBColor Light::getIntensity() const {
      return _intensity;
   }

   void Light::setIntensity(const RGBColor& intensity) {
      _intensity = intensity;
   }

   void Light::setScale(const Vector3& scale) {
      _intensity = multiplyColorByIntensity(_intensity, scale);
   }
}