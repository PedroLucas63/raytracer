#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Image/RGBColor.hpp"
#include "Core/ParamSet.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   class Light {
      protected:
         RGBColor _intensity;
         Vector3 _scale;
         
      public:
         Light(const RGBColor intensity, const Vector3 scale);
         Light(const ParamSet& params);
         virtual ~Light() = 0;

         virtual RGBColor getIntensity() const;
         virtual void setIntensity(const RGBColor& intensity);

         virtual Vector3 getScale() const;
         virtual void setScale(const Vector3& scale);
   };
}

#endif // !LIGHT_HPP
