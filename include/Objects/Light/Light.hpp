#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Image/RGBColor.hpp"
#include "Core/ParamSet.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   class Light {
      protected:
         // Stored as float Vector3 to support HDR values (scale > 1.0)
         Vector3 _intensity;
         
      public:
         Light(const RGBColor intensity, const Vector3 scale);
         Light(const ParamSet& params);
         virtual ~Light() = default;

         virtual Vector3 getIntensity() const;
         virtual void setIntensity(const RGBColor& intensity);

         virtual void setScale(const Vector3& scale);
   };
}

#endif // !LIGHT_HPP
