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

      if(params.has("attenuation")) {
         auto att = params.retrieve<Vector3>("att");
         _kc = static_cast<float>(att.getX());
         _kl = static_cast<float>(att.getY());
         _kq = static_cast<float>(att.getZ());
      }
   }

   Point3 PointLight::getPosition() const {
      return _position;
   }
   
   void PointLight::setPosition(const Point3& position) {
      _position = position;
   }

   float PointLight::computeAttenuation(float d) const {
      return 1.0f / (_kc + _kl * d + _kq * d * d);
   }
}