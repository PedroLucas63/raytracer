#include "Integrator/RayCastIntegrator.hpp"
#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Materials/FlatMaterial.hpp"
#include "Objects/Materials/GridMaterial.hpp"
#include <stdexcept>

namespace raytracer {
   std::optional<RGBColor> RayCastIntegrator::Li(const Ray& ray, const Scene& scene) const {
   //    std::optional<RGBColor> color = std::nullopt;
      
   //    std::vector<Surfel> intersections;
   //    for (const auto& primitive : scene.getPrimitives()) {
   //       if (!primitive) continue;

   //       Surfel sf(0.0f, Point3(), nullptr);

   //       auto isAggregate = std::dynamic_pointer_cast<AggregatePrimitive>(primitive) != nullptr;
   //       if (!isAggregate && !primitive->hasMaterial())
   //          continue;
         
   //       if (primitive->intersectWithSurfel(ray, &sf))
   //          intersections.push_back(sf);
   //    }

   //    if (intersections.size() > 0) {
   //       auto closestSurfel = *std::min_element(
   //          intersections.begin(), intersections.end(),
   //          [](const Surfel& a, const Surfel& b) {
   //             return a.t < b.t;
   //          }
   //       );
         
   //       auto p = closestSurfel.point;
   //       color = closestSurfel.material->getColor(p);
   //    }

   //    return color;

      Surfel surfel;

      if(!scene.intersect(ray, &surfel)) {
         return std::nullopt;
      }

      auto* flat = dynamic_cast<FlatMaterial*>(surfel.material.get());
      if (flat)
         return flat->getColor(surfel.point);
         
      std::optional<RGBColor> color = std::nullopt;

      auto* grid = dynamic_cast<GridMaterial*>(surfel.material.get());
      if (grid)
         return grid->getColor(surfel.point);

      
      if (surfel.material)
         return surfel.material->getColor(surfel.point);
 
      return std::nullopt;
      
   }
}