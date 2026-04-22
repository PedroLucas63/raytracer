#ifndef RAYCAST_INTEGRATOR_HPP
#define RAYCAST_INTEGRATOR_HPP

#include "Integrator/Integrator.hpp"

namespace raytracer {
   class RayCastIntegrator : public SamplerIntegrator {
      public:
         RayCastIntegrator() = default;
         ~RayCastIntegrator() = default;

         std::optional<RGBColor> Li(const Ray& ray, const Scene& scene) const override;
   };
}

#endif // !RAYCAST_INTEGRATOR_HPP