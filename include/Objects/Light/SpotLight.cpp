#include "Objects/Light/SpotLight.hpp"

namespace raytracer {
   SpotLight::SpotLight(
      const RGBColor intensity, 
      const Vector3 scale, 
      const Point3 from, 
      const Point3 to, 
      const float cutoff, 
      const float falloff
   ) : Light(intensity, scale), _from(from), _cutoff(cutoff), _falloff(falloff) {
      _direction = (to - from).normalize();
   }

   SpotLight::SpotLight(const ParamSet& params) : Light(params) {
      Point3 from, to;
      if (params.has("from") && params.has("to")) {
         from = params.retrieve<Point3>("from");
         to = params.retrieve<Point3>("to");
      } else {
         throw std::invalid_argument("Spot Light requires 'from' and 'to' parameters of type Point3"); 
      }

      _from = from;
      _direction = (to - from).normalize();

      _cutoff = params.retrieveOrDefault("cutoff", 0.0f);
      _falloff = params.retrieveOrDefault("falloff", 0.0f);
   }

   float SpotLight::getAttenuation(const Point3& point) const {
      auto objectDir = (point - _from).normalize();
      auto angle = _direction.angleBetween(objectDir);

      if (angle <= _falloff) return 1.0f;
      else if (angle >= _cutoff) return 0.0f;

      auto diffAngle = angle - _falloff;
      auto diffCutoff = _cutoff - _falloff;

      return 1.0f - (diffAngle / diffCutoff);
   }

   double SpotLight::getTMax(const Point3& point) const {
      return (_from - point).length();
   }

   Vector3 SpotLight::getDirectionByPoint(const Point3& point) const {
      return (_from - point).normalize();
   }
}