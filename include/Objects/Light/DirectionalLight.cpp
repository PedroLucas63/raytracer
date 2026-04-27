#include "Objects/Light/DirectionalLight.hpp"

namespace raytracer {
   DirectionalLight::DirectionalLight(const ParamSet& params) : Light(params) {
      if (!params.has("from")) {
         throw std::invalid_argument("DirectionalLight requires 'from' parameter");
      }
      auto from = params.retrieve<Point3>("from");

      if (!params.has("to")) {
         throw std::invalid_argument("DirectionalLight requires 'to' parameter");
      }
      auto to = params.retrieve<Point3>("to");

      setDirection(from, to);
   }

   Vector3 DirectionalLight::getDirection() const {
      return _direction;
   }

   void DirectionalLight::setDirection(const Vector3& direction) {
      _direction = direction.normalize();
   }

   void DirectionalLight::setDirection(const Point3& from, const Point3& to) {
      _direction = (to - from).normalize();
   }
}