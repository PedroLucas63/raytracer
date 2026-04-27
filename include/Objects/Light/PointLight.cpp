#include "Objects/Light/PointLight.hpp"

namespace raytracer {
   PointLight::PointLight(
      const RGBColor intensity, 
      const Vector3 scale, 
      const Point3 position) : Light(intensity, scale), _position(position) {}

   PointLight::PointLight(const ParamSet& params) : Light(params) {
      if (params.has("from")) {
         _position = params.retrieve<Point3>("from");
      } else {
         throw std::invalid_argument("PointLight requires a 'from' parameter of type Point3");
      }
   }

   Point3 PointLight::getPosition() const {
      return _position;
   }
   
   void PointLight::setPosition(const Point3& position) {
      _position = position;
   }
}