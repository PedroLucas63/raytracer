#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include "Objects/Light/Light.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   class SpotLight : public Light {
      private:
         Point3 _from;
         Vector3 _direction;
         float _cutoff, _falloff;
      
      public:
         SpotLight(
            const RGBColor intensity, 
            const Vector3 scale, 
            const Point3 from, 
            const Point3 to, 
            const float cutoff, 
            const float falloff
         );
         SpotLight(const ParamSet& params);
         ~SpotLight() override = default;

         float getAttenuation(const Point3& point) const override;
         double getTMax(const Point3& point) const override;
         Vector3 getDirectionByPoint(const Point3& point) const override;
   };
}

#endif // !SPOT_LIGHT_HPP