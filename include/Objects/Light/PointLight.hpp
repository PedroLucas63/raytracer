#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "Objects/Light/Light.hpp"
#include "Math/Point3.hpp"

namespace raytracer {
   class PointLight : public Light {
      private:
         Point3 _position;
         float _kc = 1.0f; // Constant attenuation factor
         float _kl = 0.0f; // Linear attenuation factor
         float _kq = 0.0f; // Quadratic attenuation factor

         float computeAttenuation(float d) const;
         
      public:
         PointLight(const RGBColor intensity, const Vector3 scale, const Point3 position);
         PointLight(const ParamSet& params);
         ~PointLight() override = default;

         Point3 getPosition() const;
         void setPosition(const Point3& position);
         
         float getAttenuation(const Point3& point) const override;
      };
}

#endif // !POINT_LIGHT_HPP