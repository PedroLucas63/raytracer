#ifndef AMBIENT_LIGHT_HPP
#define AMBIENT_LIGHT_HPP

#include "Objects/Light/Light.hpp"

namespace raytracer {
   class AmbientLight : public Light {
      public:
         AmbientLight(const ParamSet& params);
         ~AmbientLight() override = default;
   };
}

#endif // !AMBIENT_LIGHT_HPP