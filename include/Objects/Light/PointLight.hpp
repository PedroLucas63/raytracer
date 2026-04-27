#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "Objects/Light/Light.hpp"
#include "Math/Point3.hpp"

namespace raytracer {
   class PointLight : public Light {
      private:
         Point3 _position;
         
      public:
         PointLight(const RGBColor intensity, const Vector3 scale, const Point3 position);
         PointLight(const ParamSet& params);
         ~PointLight() override = default;

         Point3 getPosition() const;
         void setPosition(const Point3& position);
   };
}

#endif // !POINT_LIGHT_HPP