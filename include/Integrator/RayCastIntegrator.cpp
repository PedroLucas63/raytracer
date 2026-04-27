#include "Integrator/RayCastIntegrator.hpp"
#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Materials/FlatMaterial.hpp"
#include "Objects/Materials/GridMaterial.hpp"
#include <stdexcept>

namespace raytracer {
   std::optional<RGBColor> RayCastIntegrator::Li(const Ray& ray, const Scene& scene) const {

      Surfel surfel;

      if(!scene.intersect(ray, &surfel)) {
         return std::nullopt;
      }
      
      if (!surfel.material)
         return std::nullopt;
      
      return surfel.material->getColor(surfel.point);
   }
}