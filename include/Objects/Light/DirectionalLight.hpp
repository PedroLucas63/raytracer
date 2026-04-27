#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

#include "Objects/Light/Light.hpp"
#include "Math/Point3.hpp"

namespace raytracer {
   class DirectionalLight : public Light {
      private:
         Vector3 _direction;

      public:
         DirectionalLight(const ParamSet& params);
         ~DirectionalLight() override = default;

         Vector3 getDirection() const;
         void setDirection(const Vector3& direction);
         void setDirection(const Point3& from, const Point3& to);
   };
}

#endif // !DIRECTIONAL_LIGHT_HPP